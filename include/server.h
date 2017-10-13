#pragma once


/* Synchronizes with the client's
* 'sync_dir_<username>' directory
*/
void sync_server();


/* Receives a file sent by the client
*
*  file: file path
*/
void receive_file(char *file);


/* Sends a file to the client
*
*  file: file path
*/
void send_file(char *file);