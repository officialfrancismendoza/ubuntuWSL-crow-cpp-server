#include "crow.h"
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Define a book struct
struct Book {
	string title;
	string author;
	string published_date;
	int ratings;
	string deweyDecimalNumber;

	// Constructor with default initialization
	Book(const string& title, const string& author, const string& published_date, int ratings, const string& deweyDecimalNumber) : title(title), author(author), published_date(published_date),ratings(ratings), deweyDecimalNumber(deweyDecimalNumber) {}
};

vector<Book> books;

// Helper function to search books based on query parameters
std::vector<Book> search_books(const crow::request& req) {
    std::vector<Book> result;

    auto author = req.url_params.get("author");
    auto published_date = req.url_params.get("published_date");
    auto ratings = req.url_params.get("ratings");
    auto deweyDecimalNumber = req.url_params.get("deweyDecimalNumber");

    for (const auto& book : books) {
        if (author && book.author != author) continue;
        if (published_date && book.published_date != published_date) continue;
        if (ratings && std::to_string(book.ratings) != ratings) continue;
        if (deweyDecimalNumber && book.deweyDecimalNumber != deweyDecimalNumber) continue;

        result.push_back(book);
    }

    return result;
}

int main() {
    crow::SimpleApp app;

    // Endpoint to add a new book
    CROW_ROUTE(app, "/books").methods("POST"_method)([](const crow::request& req) {
        auto json = crow::json::load(req.body);
        if (!json || !json["title"] || !json["author"] || !json["published_date"] || !json["ratings"].i() || !json["deweyDecimalNumber"]) {
            return crow::response(400, "Missing information");
        }

        books.emplace_back(json["title"].s(), json["author"].s(), json["published_date"].s(), json["ratings"].i(), json["deweyDecimalNumber"].s());
        return crow::response(201, "Book added");
    });

    // Endpoint to search for books
    CROW_ROUTE(app, "/books").methods("GET"_method)([](const crow::request& req) {
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
    });

    app.port(18090).multithreaded().run();
}
