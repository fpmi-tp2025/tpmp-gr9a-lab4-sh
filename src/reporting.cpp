// reporting.cpp
#include "reporting.h"
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;

Reporting::Reporting(sqlite3* db) : Database(db) {}

void Reporting::generate_jockey_report(const ReportParams& params) {
    if(!params.jockey_id.has_value()) {
        throw invalid_argument("Не указан ID жокея");
    }
    
    string sql = 
        "SELECT r.date, h.name as horse_name, r.position, r.race_number, j.surname "
        "FROM races r "
        "JOIN horses h ON r.horse_id = h.id "
        "JOIN jockeys j ON r.jockey_id = j.id "
        "WHERE j.id = ? "
        "AND r.date BETWEEN ? AND ? "
        "ORDER BY r.date DESC;";
    
    cout << "\nОтчет по жокею ID:" << params.jockey_id.value() << endl;
    cout << "Период: " << params.start_date << " - " << params.end_date << "\n\n";
    cout << setw(12) << "Дата" 
         << setw(20) << "Лошадь" 
         << setw(10) << "Место" 
         << setw(8) << "Забег" 
         << setw(20) << "Жокей\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(12) << sqlite3_column_text(stmt, 0)
             << setw(20) << sqlite3_column_text(stmt, 1)
             << setw(10) << sqlite3_column_int(stmt, 2)
             << setw(8) << sqlite3_column_int(stmt, 3)
             << setw(20) << sqlite3_column_text(stmt, 4)
             << endl;
    }, {
        params.jockey_id.value(),
        params.start_date,
        params.end_date
    });
}

void Reporting::generate_owner_report(const ReportParams& params) {
    if(!params.owner_id.has_value()) {
        throw invalid_argument("Не указан владелец");
    }
    
    string sql = 
        "SELECT h.name, r.date, r.position, r.race_number, j.surname "
        "FROM horses h "
        "JOIN races r ON h.id = r.horse_id "
        "JOIN jockeys j ON r.jockey_id = j.id "
        "WHERE h.owner = ? "
        "AND r.date BETWEEN ? AND ? "
        "ORDER BY r.date DESC;";
    
    cout << "\nОтчет по владельцу: " << params.owner_id.value() << endl;
    cout << "Период: " << params.start_date << " - " << params.end_date << "\n\n";
    cout << setw(20) << "Лошадь" 
         << setw(12) << "Дата" 
         << setw(8) << "Место" 
         << setw(8) << "Забег" 
         << setw(20) << "Жокей\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(20) << sqlite3_column_text(stmt, 0)
             << setw(12) << sqlite3_column_text(stmt, 1)
             << setw(8) << sqlite3_column_int(stmt, 2)
             << setw(8) << sqlite3_column_int(stmt, 3)
             << setw(20) << sqlite3_column_text(stmt, 4)
             << endl;
    }, {
        params.owner_id.value(),
        params.start_date,
        params.end_date
    });
}

void Reporting::generate_financial_report(const ReportParams& params) {
    string sql = 
        "SELECT p.race_id, p.total_amount, "
        "       json_extract(p.distribution, '$.1st') as first, "
        "       json_extract(p.distribution, '$.2nd') as second, "
        "       json_extract(p.distribution, '$.3rd') as third, "
        "       r.date, COUNT(r.id) as participants "
        "FROM prize_funds p "
        "JOIN races r ON p.race_id = r.id "
        "WHERE r.date BETWEEN ? AND ? "
        "GROUP BY p.race_id "
        "ORDER BY r.date DESC;";
    
    cout << "\nФинансовый отчет\n";
    cout << "Период: " << params.start_date << " - " << params.end_date << "\n\n";
    cout << setw(8) << "Забег" 
         << setw(12) << "Дата" 
         << setw(12) << "Фонд" 
         << setw(12) << "1-е место" 
         << setw(12) << "2-е место" 
         << setw(12) << "3-е место" 
         << setw(10) << "Участники\n";
    
    execute(sql, [](sqlite3_stmt* stmt) {
        cout << setw(8) << sqlite3_column_int(stmt, 0)
             << setw(12) << sqlite3_column_text(stmt, 5)
             << setw(12) << sqlite3_column_double(stmt, 1)
             << setw(12) << sqlite3_column_double(stmt, 2)
             << setw(12) << sqlite3_column_double(stmt, 3)
             << setw(12) << sqlite3_column_double(stmt, 4)
             << setw(10) << sqlite3_column_int(stmt, 6)
             << endl;
    }, {
        params.start_date,
        params.end_date
    });
}

void Reporting::export_to_csv(const string& report_data, const string& filename) {
    ofstream file(filename);
    if(!file.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи");
    }
    
    // Заголовки для примера (должны соответствовать реальным данным)
    file << "Дата,Лошадь,Жокей,Место,Призовая сумма\n";
    file << report_data;
    
    cout << "Отчет успешно экспортирован в " << filename << endl;
}