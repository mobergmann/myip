#include <iostream>
#include <stdio.h>
#include <curl/curl.h>
#include <string>

using namespace std;

/**
 * A struct for managing an IP object.
 * Contains the ip, the country and the cc as a string.
 */
struct IP
{
	string ip;
	string country;
	string cc;
};

/**
 * For writing data to a string
 */
static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);

/**
 * make an API request to https://api.myip.com and return the string
 */
static string get_response();

/**
 * extracts a word until the next occurrence of a " symbol.
 */
static string extract_entry(int starting_position, string original);

/**
 * extracts an ip struct from an string.
 */
static IP parse_IP(string input);

/**
 * prints an ip struct to the console.
 */
static void output(IP ip);

int main(int argc, char* argv[])
{
	string respone = get_response();

	IP ip = parse_IP(respone);

	output(ip);

	return 0;
}

static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
	((string*)stream)->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

static string get_response()
{
	CURL* curl_handle;

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();

	string readBuffer;

	curl_easy_setopt(curl_handle, CURLOPT_URL, "https://api.myip.com");
	/* send all data to this function  */
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &readBuffer);

	/* get it! */
	curl_easy_perform(curl_handle);

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);

	return readBuffer;
}

static IP parse_IP(string input)
{
	IP ip;

	for (int i = 0; i < input.length(); i++)
	{
		if (
			// if begins with "i => ip
			ip.ip.length() == 0 &&
			input[i] == '\"' &&
			input[i + 1] == 'i')
		{
			i += 6;
			ip.ip = extract_entry(i, input);
		}
		else if (
			// if begins with "co => Country
			ip.country.length() == 0 &&
			input[i] == '\"' &&
			input[i + 1] == 'c' &&
			input[i + 2] == 'o')
		{
			i += 11;
			ip.country = extract_entry(i, input);
		}
		else if (
			// if begins with "c => cc (cannot be "co")
			ip.cc.length() == 0 &&
			input[i] == '\"' &&
			input[i + 1] == 'c')
		{
			i += 6;
			ip.cc = extract_entry(i, input);
		}
	}

	return ip;
}

static string extract_entry(int starting_position, string original)
{
	string ret = "";
	for (int i = starting_position; original[i] != '\"'; i++)
	{
		ret += original[i];
	}
	return ret;
}

static void output(IP ip)
{
	cout << ((ip.ip.length() != 0) ? ("IP: " + ip.ip + "\r\n") : "") +
		((ip.country.length() != 0) ? ("Country: " + ip.country + "\r\n") : "") +
		((ip.cc.length() != 0) ? ("cc: " + ip.cc + "\r\n") : "");
}
