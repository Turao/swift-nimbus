#pragma once

/* Connects the client to the server
*  host: server address
*  port: server port
*/
int connect_server(char *host, int port);


/* Synchronizes 'sync_dir_<username>' directory
*  with the server
*/
void sync_client();


/* Uploads a local file to the server
*
*  file: file path
*/
void send_file(char *file);


/* Downloads a file from the server
*
*  file: file path
*/
void get_file(char *file);


/* Deletes a file from the local directory
*  'sync_dir_<username>'
*
*  file: file path
*/
void delete_file(char* file);


/* Closes the connection with the server
*/
void close_connection();