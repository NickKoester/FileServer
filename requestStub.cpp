


//I think we should move everything after we accept() into
//this function.  This function could handle parsing the message
//and calling the specific request function.  Then after the request
//returns we can send the response in this function. That way we can make
//this its own thread.
void requestStub(int sockfd, char *message) {
// TODO
// 1. Either pass the message in or recv() it in here
// 2. Decrypt
// 3. Parse request
// 4. Figure out which type of request it is.
// 5. Verify inputs and shit
// 6. Make Path object (look at Path.h for details)
// 7. Call appropriate function
// 8. Send response message

}
