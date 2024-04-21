#pragma once

#include <Arduino.h>
#include <MainConfig.h>

#include <SPI.h>
#include <SD.h>
//extern SDClass globalSDcard0;
extern SDFileSystemClass globalSDcard0;

/***
 * gloabal SD card is the project access point to SD card
 * 
 * the object is designed to suppport multiple parallel file operations  ==> realy ?? some comments of some functions give a different statement 
 * but not supported is to acces one file from different points in the program
 * 
 * 
 * 
*/

