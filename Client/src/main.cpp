// #include <asio.hpp>
// #include <print>
// #include <span>
// #include <string>
// #include <vector>

// using asio::ip::tcp;

// std::vector<std::string> requests = {
//     {"GET / HTTP/1.1\r\n"s
//      "Host: localhost\r\n"s
//      "Connection: close\r\n"s
//      "\r\n"s},

//     {"POST /users HTTP/1.1\r\n"s
//      "Host: example.com\r\n"s
//      "Content-Type: application/x-www-form-urlencoded\r\n"s
//      "Content-Length: 49\r\n"s
//      "\r\n"s
//      "name=FirstName+LastName&email=bsmth%40example.com\r\n"s
//      "\r\n"s},

//     {"GET /index.html HTTP/1.1\r\n"s
//      "Host: www.example.com\r\n"s
//      "User-Agent: Mozilla/5.0\r\n"s
//      "Accept: text/html\r\n"s
//      "\r\n"s},

//     {"GET /hello.htm HTTP/1.1\r\n"s
//      "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"s
//      "Host: www.tutorialspoint.com\r\n"s
//      "Accept-Language: en-us\r\n"s
//      "Accept-Encoding: gzip, deflate\r\n"s
//      "Connection: Keep-Alive\r\n"s
//      "\r\n"s},

//     {"POST /cgi-bin/process.cgi HTTP/1.1\r\n"s
//      "User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\n"s
//      "Host: www.tutorialspoint.com\r\n"s
//      "Content-Type: text/xml; charset = utf - 8\r\n"s
//      "Content-Length: 60\r\n"s
//      "Accept-Language: en-us\r\n"s
//      "Accept-Encoding: gzip, deflate\r\n"s
//      "Connection: Keep-Alive\r\n"s
//      "\r\n"s
//      "first=Zara&last=Ali\r\n"s
//      "\r\n"s}};

// constexpr static std::size_t kBufferSize = 4096;
// std::array<char, kBufferSize> buffer;

// void GrabSomeData(tcp::socket &socket) {
//   socket.async_read_some(
//       asio::buffer(buffer.data(), buffer.size()),
//       [&](asio::error_code error_code, std::size_t length) {
//         if (!error_code) {
//           std::println("\n\nRead {} bytes\n\n", length);
//           std::println("{}", std::string(buffer.data(), buffer.size()));

//           GrabSomeData(socket);
//         }
//       });
// }

// int main(int argc, char *argv[]) {
//   std::vector<std::string> args;
//   for (const auto *el : std::span(argv, argc)) {
//     args.emplace_back(el);
//     std::println("{}", el);
//   }

//   asio::error_code error_code;

//   asio::io_context io_context;
//   asio::io_context::work idle_work(io_context);

//   std::thread thread = std::thread([&]() { io_context.run(); });

//   tcp::resolver resolver(io_context);
//   auto endpoints = resolver.resolve("127.0.0.1", "80");
//   tcp::socket socket(io_context);

//   asio::connect(socket, endpoints);

//   if (!error_code) {
//     std::println("Connected");
//   } else {
//     std::println("failed to connect to address: {}", error_code.message());
//   }

//   if (socket.is_open()) {
//     GrabSomeData(socket);

//     std::string request = std::format(
//         "GET / HTTP/1.1\r\n"
//         "Host: {}\n"
//         "Connection: close\r\n\r\n",
//         args[1]);
//     socket.write_some(asio::buffer(request), error_code);

//   using namespace std::chrono_literals;
//   std::this_thread::sleep_for(20000ms);

//   io_context.stop();
//   if (thread.joinable()) {
//     thread.join();
//   }
// }

//   return EXIT_SUCCESS;
// }

int main() { return 0; }
