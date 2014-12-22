
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define SAFE_FREE(s) if(s){free((void*)s);s=NULL;}

const char * _appKey = NULL;
const char * _appSecret = NULL;
const char * _token = NULL;
const char * _tokenSecret = NULL;
const char * _fileName = NULL;
const char * _filePath = NULL;
FILE * _sendFile = NULL;

void cleanParams()
{
	if (_sendFile)
	{
		fclose(_sendFile);
		_sendFile = NULL;
	}
	SAFE_FREE(_appKey)
	SAFE_FREE(_appSecret)
	SAFE_FREE(_token)
	SAFE_FREE(_tokenSecret)
	SAFE_FREE(_fileName)
	SAFE_FREE(_filePath)
}
 
size_t readFile(void * ptr, size_t size, size_t nmeb, void * stream)
{
	return (ptr && stream) ? fread(ptr, size, nmeb, (FILE *)stream) : 0;
}

const char * copyString(const char * src)
{
	if (src) 
	{
		const size_t len = strlen(src);
		char * s = (char *)malloc(len + 1);
		if (s) 
		{
			memcpy(s, src, len);
			s[len] = (char)0;
			return s;
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	argc--; argv++;
	
	while (argc)
	{
		const char * p = copyString(*argv++);
		const char * v = copyString(*argv++);
		if (p && v) 
		{
			if (strcmp(p, "-fp") == 0) _filePath = copyString(v);
			else if (strcmp(p, "-fn") == 0) _fileName = copyString(v);
			else if (strcmp(p, "-ak") == 0) _appKey = copyString(v);
			else if (strcmp(p, "-as") == 0) _appSecret = copyString(v);
			else if (strcmp(p, "-t") == 0) _token = copyString(v);
			else if (strcmp(p, "-ts") == 0) _tokenSecret = copyString(v);
		}
		SAFE_FREE(p)
		SAFE_FREE(v)
		argc-= 2;
	}
	
	if (!_appKey || !_appSecret || !_token || !_tokenSecret || !_fileName || !_filePath)
	{
		fprintf(stdout, "Format: -fp FILE_PATH -fn FILE_NAME -ak APP_KEY -as APP_SECRET -t TOKEN -ts TOKEN_SECRET");
		cleanParams();
		return EXIT_SUCCESS;
	}
	
	CURL * curl = NULL;
	CURLcode res = 0;
	
	_sendFile = fopen(_filePath, "r+b");
	if (!_sendFile)
	{
		fprintf(stderr, "Can't open file: %s", _filePath);
		cleanParams();
		return EXIT_FAILURE;
	}
	
	curl = curl_easy_init();
	if (!curl) 
	{
		fprintf(stderr, "Can't initialize curl.");
		cleanParams();
		return EXIT_FAILURE;
	}
	
	char url[1024] = { 0 };
	if (sprintf(url, "https://api-content.dropbox.com/1/files_put/sandbox/%s", _fileName) <= 0)
	{
		fprintf(stderr, "Can't initialize url string.");
		cleanParams();
		return EXIT_FAILURE;
	}
	
	curl_easy_setopt(curl, CURLOPT_URL, url);
	
	struct curl_slist * headers = NULL;
	
	char header[2048] = { 0 };
	if (sprintf(header, "Authorization: OAuth oauth_version=\"1.0\", oauth_signature_method=\"PLAINTEXT\", oauth_consumer_key=\"%s\", oauth_token=\"%s\", oauth_signature=\"%s&%s\"",
			_appKey, _token, _appSecret, _tokenSecret) <= 0)
	{
		fprintf(stderr, "Can't initialize header.");
		cleanParams();
		return EXIT_FAILURE;
	}
	headers = curl_slist_append(headers, header);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	
	curl_easy_setopt(curl, CURLOPT_PUT, 1L);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, readFile);
	curl_easy_setopt(curl, CURLOPT_READDATA, _sendFile);
	
	res = curl_easy_perform(curl);
	
	curl_slist_free_all(headers);
	if(res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		cleanParams();
		return EXIT_FAILURE;
	}
	
	curl_easy_cleanup(curl);
	
	cleanParams();
	
	return EXIT_SUCCESS;
}