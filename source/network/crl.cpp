#include "crl.h"

size_t writeChunk(void *data, size_t size, size_t nmemb, void *userdata) {
    size_t real_size = size * nmemb;

    Response* response = (Response*) userdata;

    char *ptr = (char*)realloc(response->string, response->size + real_size + 1);
    if (ptr == NULL)
        return -1;
    
    response->string = ptr;
    memcpy(&(response->string[response->size]), data, real_size);
    response->size += real_size;
    response->string[response->size] = 0;
    return real_size;
}

std::string sendHTTPRequest(std::string test_url, Output* output) {
    CURL *curl;
    CURLcode result;
    curl = curl_easy_init();
    std::stringstream debug;

    if (curl == NULL) {
        // output->print("HTTP request failed.");
        return "";
    }

    Response response;
    response.string = (char*)malloc(1);
    response.size = 0;

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_URL, test_url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (iPhone; CPU iPhone OS 5_0 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9A334 Safari/7534.48.3");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeChunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        debug << "Error: " << curl_easy_strerror(result);
        // output->print(debug.str());
        return "";
    }
    
    debug << response.string;

    curl_easy_cleanup(curl);

    free(response.string);

    return debug.str();
}