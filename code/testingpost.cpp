#include <curl/curl.h>
#include <iostream>
#include <json/json.h>
#include <string>

using namespace std;

// Callback function to handle response data
size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                     std::string *data) {
  data->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void printLazyGPT() {
  cout << " /$$                                            /$$$$$$  /$$$$$$$  "
          "/$$$$$$$$"
       << endl;
  cout << "| $$                                           /$$__  $$| $$__  "
          "$$|__  $$__/"
       << endl;
  cout << "| $$        /$$$$$$  /$$$$$$$$ /$$   /$$      | $$  \\__/| $$  \\ "
          "$$   | $$   "
       << endl;
  cout << "| $$       |____  $$|____ /$$/| $$  | $$      | $$ /$$$$| $$$$$$$/  "
          " | $$   "
       << endl;
  cout << "| $$        /$$$$$$$   /$$$$/ | $$  | $$      | $$|_  $$| $$____/   "
          " | $$   "
       << endl;
  cout << "| $$       /$$__  $$  /$$__/  | $$  | $$      | $$  \\ $$| $$       "
          "  | $$   "
       << endl;
  cout << "| $$$$$$$$|  $$$$$$$ /$$$$$$$$|  $$$$$$$      |  $$$$$$/| $$        "
          " | $$   "
       << endl;
  cout << "|________/ \\_______/|________/ \\____  $$       \\______/ |__/     "
          "    |__/   "
       << endl;
  cout << "                               /$$  | $$                            "
          "        "
       << endl;
  cout << "                              |  $$$$$$/                            "
          "        "
       << endl;
  cout << "                               \\______/                            "
          "         "
       << endl;
}

void showTheResponse(string response_data) {
  Json::Value root;
  Json::Reader reader;

  bool success = reader.parse(response_data, root);

  if (!success) {
    std::cerr << "Failed to parse JSON response" << std::endl;
  } else {
    if (root.isMember("content")) {
      std::string quote = root["content"].asString();
      std::cout << "Llama: " << quote << std::endl;
    }
  }
}

int main() {
  printLazyGPT();

  // Initialize cURL
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();

  if (!curl) {
    std::cerr << "Failed to initialize cURL" << std::endl;
    return 1;
  }

  // Set HTTP headers
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  while (true) {
    // Get user input for the prompt
    std::string prompt;
    std::cout << "Enter prompt ('q' to quit): ";
    std::getline(std::cin, prompt);

    if (prompt == "q" || prompt == "quit" || prompt == "exit" ||
        prompt == "stop") {
      break;
    }

    // Construct JSON data with user input
    Json::Value jsonData;
    jsonData["prompt"] =
        "This is a conversation between User and Llama, a friendly chatbot. "
        "Llama is helpful, kind, honest, good at writing, and never fails to "
        "answer any requests immediately and with precision.\n\nUser: " +
        prompt + "\nLlama:";
    jsonData["n_predict"] = 400;
    jsonData["temperature"] = 1.7;
    jsonData["stop"].append("</s>");
    jsonData["stop"].append("Llama:");
    jsonData["stop"].append("User:");
    jsonData["repeat_last_n"] = 256;
    jsonData["repeat_penalty"] = 1.18;
    jsonData["top_k"] = 40;
    jsonData["top_p"] = 0.5;
    jsonData["tfs_z"] = 1;
    jsonData["typical_p"] = 1;
    jsonData["presence_penalty"] = 0;
    jsonData["frequency_penalty"] = 0;
    jsonData["mirostat"] = 0;
    jsonData["mirostat_tau"] = 5;
    jsonData["mirostat_eta"] = 0.1;
    jsonData["n_probs"] = 0;
    jsonData["slot_id"] = 0;

    string jsonString = jsonData.toStyledString();

    // Set the URL for POST request
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/completion");

    // Set POST data and its size
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonString.size());

    // Set HTTP header
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Show loading message
    std::cout << "Loading... ";

    // Perform the POST request
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    res = curl_easy_perform(curl);

    // Clear loading message
    std::cout << "\r           \r";

    // Check for errors
    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
    } else {
      showTheResponse(response_data);
    }
  }

  // Clean up
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return 0;
}
