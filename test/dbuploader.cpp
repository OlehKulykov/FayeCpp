
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <curl/curl.h>

using namespace std;

size_t readFile(void * ptr, size_t size, size_t nmeb, void * stream)
{
	return fread(ptr, size, nmeb, (FILE *)stream);
}

int main(int argc, char **argv)
{
	argc--; argv++;
	
	string appKey;
	string appSecret;
	string token;
	string tokenSecret;
	string fileName;
	string filePath;
	
	while (argc)
	{
		const string p = *argv++;
		const string v = *argv++;
		
		if (p == "-fp") filePath = v;
		else if (p == "-fn") fileName = v;
		else if (p == "-ak") appKey = v;
		else if (p == "-as") appSecret = v;
		else if (p == "-t") token = v;
		else if (p == "-ts") tokenSecret = v;
		
		argc-= 2;
	}
	
	if (!appKey.length() || !appSecret.length() || !token.length() || !tokenSecret.length() || !fileName.length() || !filePath.length())
	{
		fprintf(stdout, "Format: -fp FILE_PATH -fn FILE_NAME -ak APP_KEY -as APP_SECRET -t TOKEN -ts TOKEN_SECRET");
		return EXIT_SUCCESS;
	}
	
	
	CURL * curl = NULL;
	CURLcode res;
	
	FILE * rfp = fopen(filePath.c_str(), "r+b");
	if (!rfp)
	{
		fprintf(stderr, "Can't open file: %s", filePath.c_str());
		return EXIT_FAILURE;
	}
	
	curl = curl_easy_init();
	if (!curl) 
	{
		fprintf(stderr, "Can't initialize curl.");
		return EXIT_FAILURE;
	}
	
	
	string url = "https://api-content.dropbox.com/1/files_put/sandbox/";
	url += fileName;
	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	
	struct curl_slist * headers = NULL;
	string header = "Authorization: OAuth oauth_version=\"1.0\", oauth_signature_method=\"PLAINTEXT\", oauth_consumer_key=\"" + appKey + "\", oauth_token=\"" + token + "\", oauth_signature=\"" + appSecret + "&" + tokenSecret + "\"";
	headers = curl_slist_append(headers, header.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	
	curl_easy_setopt(curl, CURLOPT_PUT, 1L);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, readFile);
	curl_easy_setopt(curl, CURLOPT_READDATA, rfp);
	
	res = curl_easy_perform(curl);
	
	curl_slist_free_all(headers);
	if(res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return EXIT_FAILURE;
	}
	
	curl_easy_cleanup(curl);
	
	fclose(rfp);
	
	return EXIT_SUCCESS;
}