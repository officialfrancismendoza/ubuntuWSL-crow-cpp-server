#include "crow.h"
#include <unordered_map>
#include <vector>
#include <string>

struct Book {
    std::string title;
    std::string author;
    std::string published_date; // Keeping it simple as a string for now
    int ratings;
    std::string deweyDecimalNumber;

    Book(const std::string& title, const std::string& author, const std::string& published_date, int ratings, const std::string& deweyDecimalNumber) 
        : title(title), author(author), published_date(published_date), ratings(ratings), deweyDecimalNumber(deweyDecimalNumber) {}
};

// Use unordered_map with deweyDecimalNumber as the key
std::unordered_map<std::string, Book> books;

// Helper function for O(N) search
std::vector<Book> search_books(const crow::request& req) {
    std::vector<Book> result;
    for (const auto& pair : books) {
        const auto& book = pair.second;
        bool match = true;
        for (const auto& param : req.url_params) {
            if (param.key == "author" && book.author != param.value) match = false;
            else if (param.key == "published_date" && book.published_date != param.value) match = false;
            else if (param.key == "ratings" && std::to_string(book.ratings) != param.value) match = false;
            // Skip deweyDecimalNumber as it's used for O(1) search
        }
        if (match) result.push_back(book);
    }
    return result;
}

int main() {
    crow::SimpleApp app;

    // POST endpoint to add a new book
    CROW_ROUTE(app, "/books").methods(crow::HTTPMethod::Post)([](const crow::request& req) {
        auto json = crow::json::load(req.body);
        if (!json || json["deweyDecimalNumber"].s().empty() || books.find(json["deweyDecimalNumber"].s()) != books.end()) {
            return crow::response(400, "Missing deweyDecimalNumber or book already exists");
        }
        
        books[json["deweyDecimalNumber"].s()] = Book(json["title"].s(), json["author"].s(), json["published_date"].s(), json["ratings"].i(), json["deweyDecimalNumber"].s());
        return crow::response(201, "Book added");
    });

    // GET endpoint for searching books
    CROW_ROUTE(app, "/books").methods(crow::HTTPMethod::Get)([](const crow::request& req) {
        if (auto dewey = req.url_params.get("deweyDecimalNumber"); dewey) {
            if (books.find(dewey) != books.end()) {
                const auto& book = books[dewey];
                crow::json::wvalue dto;
                dto["title"] = book.title;
                dto["author"] = book.author;
                dto["published_date"] = book.published_date;
                dto["ratings"] = book.ratings;
                dto["deweyDecimalNumber"] = book.deweyDecimalNumber;
                return crow::response{dto};
            }
            return crow::response(404, "Book not found");
        } else {
            auto result = search_books(req);
            crow::json::wvalue dto;
            for (size_t i = 0; i < result.size(); ++i) {
                dto[i]["title"] = result[i].title;
                dto[i]["author"] = result[i].author;
                dto[i]["published_date"] = result[i].published_date;
                dto[i]["ratings"] = result[i].ratings;
                dto[i]["deweyDecimalNumber"] = result[i].deweyDecimalNumber;
            }
            return crow::response{dto};
        }
    });

    app.port(18080).multithreaded().run();
}
