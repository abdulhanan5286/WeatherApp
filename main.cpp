#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

#pragma comment(lib, "winhttp.lib")


wstring stringToWstring(const string& str)
{
    return wstring(str.begin(), str.end());
}

int main()
{
    string city;
    cout << "Enter city name: ";
    getline(cin, city);  

    
    wstring path = L"/data/2.5/weather?q=" + stringToWstring(city) +
        L"&appid=0c7d7efe508849ac56839ac0fda09b3b&units=metric";

    wstring server = L"api.openweathermap.org";
    string responseText;

    HINTERNET hSession = WinHttpOpen(L"WeatherApp",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession)
    {
        HINTERNET hConnect = WinHttpConnect(hSession, server.c_str(),
            INTERNET_DEFAULT_HTTP_PORT, 0);

        if (hConnect)
        {
            HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
                NULL, WINHTTP_NO_REFERER,
                WINHTTP_DEFAULT_ACCEPT_TYPES,
                0);

            if (hRequest)
            {
                BOOL bResults = WinHttpSendRequest(hRequest,
                    WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                    WINHTTP_NO_REQUEST_DATA, 0,
                    0, 0);

                if (bResults)
                    bResults = WinHttpReceiveResponse(hRequest, NULL);

                if (bResults)
                {
                    DWORD dwSize = 0;
                    DWORD dwDownloaded = 0;
                    LPSTR pszOutBuffer;
                    BOOL bResults = TRUE;

                    do
                    {
                        dwSize = 0;
                        if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                            break;

                        pszOutBuffer = new char[dwSize + 1];
                        ZeroMemory(pszOutBuffer, dwSize + 1);

                        if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                        {
                            delete[] pszOutBuffer;
                            break;
                        }

                        responseText += pszOutBuffer;
                        delete[] pszOutBuffer;

                        if (dwDownloaded == 0)
                            break;

                    } while (dwSize > 0);
                }

                WinHttpCloseHandle(hRequest);
            }

            WinHttpCloseHandle(hConnect);
        }

        WinHttpCloseHandle(hSession);
    }

    
    try {
        json j = json::parse(responseText);

        cout << "\nWeather in " << j["name"] << ":\n";
        cout << "Temperature: " << j["main"]["temp"] << "  C\n";
        cout << "Condition: " << j["weather"][0]["description"] << "\n";
        cout << "Humidity: " << j["main"]["humidity"] << "%\n";
        cout << "Wind Speed: " << j["wind"]["speed"] << " m/s\n";
    }
    catch (...) {
        cout << "Failed to fetch or parse weather data.\n";
    }

    return 0;
}
