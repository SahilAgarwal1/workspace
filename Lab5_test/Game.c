/*
 * Game.c
 *
 *  Created on: Apr 16, 2021
 *      Author: Kyle
 */

#include "Game.h"
#include "cc3100_usage.h"
#include <stdio.h>

#define ACK 1
#define START_GAME 2

GameState_t gameState;
SpecificPlayerInfo_t clientSpecificInfo, clientInfo;

semaphore_t netMutex, gameStateMutex, clientMutex;



/*********************************************** Host Threads *********************************************************************/

// Thread for the host to create a game
void CreateGame() {

    initCC3100(Host);

    // LED indicator
    initLEDs();
    turnLedOn(LED2);

    G8RTOS_InitSemaphore(&netMutex, 1);
    G8RTOS_InitSemaphore(&gameStateMutex, 1);
    G8RTOS_InitSemaphore(&clientMutex, 1);

    G8RTOS_AddThread(IdleThread, 7, "Idle");

    // Wait for client
    int32_t retVal = NOTHING_RECEIVED;
    while (retVal == NOTHING_RECEIVED){
        G8RTOS_WaitSemaphore(&netMutex);
        retVal = ReceiveData((uint8_t *)&clientSpecificInfo, sizeof(clientSpecificInfo));
        G8RTOS_SignalSemaphore(&netMutex);
        OS_Sleep(1);
    }

    // Wait for client ACK
    uint8_t resp;
    while (resp != ACK){
        // Send ACK
        resp = ACK;
        G8RTOS_WaitSemaphore(&netMutex);
        SendData((unsigned char *)&resp, HOST_IP_ADDR, 1);
        G8RTOS_SignalSemaphore(&netMutex);
        OS_Sleep(1);

        resp = 0;
        G8RTOS_WaitSemaphore(&netMutex);
        ReceiveData((uint8_t *)&resp, sizeof(resp));
        G8RTOS_SignalSemaphore(&netMutex);
        OS_Sleep(1);
    }

    // Handshake complete

    // Send START_GAME
    uint8_t start = START_GAME;
    G8RTOS_WaitSemaphore(&netMutex);
    SendData((unsigned char *)&start, HOST_IP_ADDR, 1);
    G8RTOS_SignalSemaphore(&netMutex);

    G8RTOS_AddThread(SendDataToClient, 6, "SendDataToClient");
    G8RTOS_AddThread(ReceiveDataFromClient, 6, "ReceiveDataFromClient");

    G8RTOS_KillSelf();
}


// Thread that sends game state to client
void SendDataToClient(){
    while(1){

        // Make packet
        G8RTOS_WaitSemaphore(&gameStateMutex);
        GameState_t packet = gameState;
        G8RTOS_SignalSemaphore(&gameStateMutex);

        // Send packet
        G8RTOS_WaitSemaphore(&netMutex);
        SendData((unsigned char *)&packet, HOST_IP_ADDR, sizeof(packet));
        G8RTOS_SignalSemaphore(&netMutex);

        //* Check if game is done

        OS_Sleep(5);
    }
}


// Thread that receives UDP packets from client
void ReceiveDataFromClient(){
    while(1){
        int32_t retVal = NOTHING_RECEIVED;
        SpecificPlayerInfo_t packet;

        // Wait for data
        while (retVal == NOTHING_RECEIVED){
            G8RTOS_WaitSemaphore(&netMutex);
            retVal = ReceiveData((uint8_t *)&packet, sizeof(packet));
            G8RTOS_SignalSemaphore(&netMutex);
            OS_Sleep(1);
        }

//        LCD_Clear(LCD_RED);
//        LCD_Clear(LCD_BLACK);

        // Update the player�s current center with the displacement received from the client
        G8RTOS_WaitSemaphore(&clientMutex);
        clientInfo = packet;
//        if(clientInfo.displacement  >= 200 && (gamestate.players[1].currentCenter - PADDLE_LEN_D2 >= ARENA_MIN_X)){
//            gamestate.players[1].currentCenter -= 1;
//        }
//        else if(clientInfo.displacement <= -200 && (gamestate.players[1].currentCenter + PADDLE_LEN_D2 <= ARENA_MAX_X)){
//            gamestate.players[1].currentCenter += 1;
//        }

        G8RTOS_SignalSemaphore(&clientMutex);

        OS_Sleep(2);
    }
}


// Generate Ball thread
void GenerateBall(){

}


// Thread to read host's joystick
void ReadJoystickHost(){

}


// Thread to move a single ball
void MoveBall(){

}


// End of game for the host
void EndOfGameHost(){

    // Wait for all semaphores to be released
    G8RTOS_WaitSemaphore(&gameStateMutex);
    G8RTOS_WaitSemaphore(&netMutex);
    G8RTOS_WaitSemaphore(&clientMutex);

    // Kill all other threads

    // Re-initialize semaphores
    G8RTOS_InitSemaphore(&netMutex, 1);
    G8RTOS_InitSemaphore(&gameStateMutex, 1);
    G8RTOS_InitSemaphore(&clientMutex, 1);

    // Wait for button press

    // Send START_GAME
    uint8_t start = START_GAME;
    G8RTOS_WaitSemaphore(&netMutex);
    SendData((unsigned char *)&start, HOST_IP_ADDR, 1);
    G8RTOS_SignalSemaphore(&netMutex);

    // Add all threads back and restart game variables
    G8RTOS_AddThread(SendDataToClient, 6, "SendDataToClient");
    G8RTOS_AddThread(ReceiveDataFromClient, 6, "ReceiveDataFromClient");

    G8RTOS_KillSelf();
}

/*********************************************** Host Threads *********************************************************************/


/*********************************************** Client Threads *********************************************************************/

// Thread for client to join game
void JoinGame(){
    initCC3100(Client);

    // LED indicator
    initLEDs();
    turnLedOn(LED2);

    G8RTOS_InitSemaphore(&netMutex, 1);
    G8RTOS_InitSemaphore(&gameStateMutex, 1);
//    G8RTOS_InitSemaphore(&clientMutex, 1);

    G8RTOS_AddThread(IdleThread, 7, "Idle");

    // Set initial SpecificPlayerInfo_t strict attributes (you can get the IP address by calling getLocalIP()
    clientSpecificInfo.IP_address = getLocalIP();
    char str[15];
    sprintf(str, "%d.%d.%d.%d", (clientSpecificInfo.IP_address & 0xFF000000) >> 24, (clientSpecificInfo.IP_address & 0xFF0000) >> 16, (clientSpecificInfo.IP_address & 0xFF00) >> 8, (clientSpecificInfo.IP_address & 0xFF));
    LCD_Text(0, 0, (uint8_t*)&str, LCD_WHITE);

    // Wait for ACK
    uint8_t resp = 0;
    while (resp != ACK){
        // Send clientSpecificInfo to host
        G8RTOS_WaitSemaphore(&netMutex);
        SendData((unsigned char *)&clientSpecificInfo, HOST_IP_ADDR, sizeof(clientSpecificInfo));
        G8RTOS_SignalSemaphore(&netMutex);
        OS_Sleep(1);

        G8RTOS_WaitSemaphore(&netMutex);
        ReceiveData((uint8_t *)&resp, 1);
        G8RTOS_SignalSemaphore(&netMutex);
        OS_Sleep(1);
    }

    // Send ACK
    G8RTOS_WaitSemaphore(&netMutex);
    SendData((unsigned char *)&resp, HOST_IP_ADDR, 1);
    G8RTOS_SignalSemaphore(&netMutex);

    // Handshake complete
    turnLedOn(LED1);

    // Wait for START_GAME
    resp = 0;
    while (resp != START_GAME){
        G8RTOS_WaitSemaphore(&netMutex);
        ReceiveData((uint8_t *)&resp, 1);
        G8RTOS_SignalSemaphore(&netMutex);
        OS_Sleep(1);
    }

    G8RTOS_AddThread(ReceiveDataFromHost, 6, "ReceiveDataFromHost");
    G8RTOS_AddThread(SendDataToHost, 6, "SendDataToHost");

    G8RTOS_KillSelf();
}


// Thread that receives game state packets from host
void ReceiveDataFromHost(){
    while(1) {
        int32_t retVal = NOTHING_RECEIVED;
        GameState_t packet;

        // Wait for data
        while (retVal == NOTHING_RECEIVED){
            G8RTOS_WaitSemaphore(&netMutex);
            retVal = ReceiveData((uint8_t *)&packet, sizeof(packet));
            G8RTOS_SignalSemaphore(&netMutex);
            OS_Sleep(1);
        }

        // Update gamestate
        G8RTOS_WaitSemaphore(&gameStateMutex);
        gameState = packet;
        G8RTOS_SignalSemaphore(&gameStateMutex);

        //* Check if game is done

        OS_Sleep(5);
    }
}


// Thread that sends UDP packets to host
void SendDataToHost(){
    while(1) {
        G8RTOS_WaitSemaphore(&netMutex);
        SendData((unsigned char *)&clientInfo, HOST_IP_ADDR, sizeof(clientInfo));
        G8RTOS_SignalSemaphore(&netMutex);

        OS_Sleep(2);
    }
}

// Thread to read client's joystick

void ReadJoystickClient(){

}

// End of game for the client
void EndOfGameClient(){

    // Wait for all semaphores to be released
    G8RTOS_WaitSemaphore(&gameStateMutex);
    G8RTOS_WaitSemaphore(&netMutex);
//    G8RTOS_WaitSemaphore(&clientMutex, 1);



    // Re-initialize semaphores
    G8RTOS_InitSemaphore(&netMutex, 1);
    G8RTOS_InitSemaphore(&gameStateMutex, 1);
//    G8RTOS_InitSemaphore(&clientMutex, 1);

    // Wait for restart
    uint8_t resp = 0;
    while (resp != START_GAME){
        G8RTOS_WaitSemaphore(&netMutex);
        ReceiveData((uint8_t *)&resp, 1);
        G8RTOS_SignalSemaphore(&netMutex);
        OS_Sleep(1);
    }

    // Add all threads back and restart game variables
    G8RTOS_AddThread(ReceiveDataFromHost, 6, "ReceiveDataFromHost");
    G8RTOS_AddThread(SendDataToHost, 6, "SendDataToHost");

    G8RTOS_KillSelf();
}

/*********************************************** Client Threads *********************************************************************/


/*********************************************** Common Threads *********************************************************************/

// Idle thread
void IdleThread(){
    while(1);
}

/*********************************************** Common Threads *********************************************************************/

