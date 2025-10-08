/**
 * URescue Entry Point - Standard MFC WinMain
 *
 * This file contains the standard MFC entry point that will automatically
 * call our CUrescueApp::InitInstance() method in URescueEntryPoint.cpp.
 *
 * Entry point flow:
 * WinMain -> AfxWinMain -> CUrescueApp::InitInstance -> CUrescueApp_InitInstance
 */

#include "../include/CUrescueApp.h"
#include "../include/stdafx.h"

// MFC entry point declaration
extern int WINAPI
AfxWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

/**
 * Standard Windows Main Entry Point for MFC Applications
 * MFC will automatically call CUrescueApp::InitInstance()
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Let MFC handle the application startup
    // This will automatically create theApp and call InitInstance
    return AfxWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
