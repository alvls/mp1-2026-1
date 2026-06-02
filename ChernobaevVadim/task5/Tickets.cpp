#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

enum class Zone
{
    Regular,
    VIP
};

struct Seat
{
    int row;
    int number;
    Zone zone;
    bool occupied = false;
};

struct Ticket
{
    string date;
    string time;
    string movie;
    int hall;
    int row;
    int seat;
};

struct Session
{
    string date;
    string time;
    string movie;
    int hall;
    double basePrice;
    vector<Seat> seats;
};

class Cinema
{
private:
    vector<Session> sessions;

public:
    void addSession(Session s)
    {
        sessions.push_back(s);
    }

    Session* findSession(string date, string time, string movie, int hall)
    {
        for (auto& s : sessions)
        {
            if (s.date == date && s.time == time &&
                s.movie == movie && s.hall == hall)
                return &s;
        }
        return nullptr;
    }

    bool checkSeats(Session* session, Zone zone, int count)
    {
        int freeSeats = 0;

        for (auto& seat : session->seats)
        {
            if (!seat.occupied && seat.zone == zone)
                freeSeats++;
        }

        return freeSeats >= count;
    }

    vector<Seat*> reserveSeats(Session* session, Zone zone, int count)
    {
        vector<Seat*> result;

        for (auto& seat : session->seats)
        {
            if (!seat.occupied && seat.zone == zone)
            {
                seat.occupied = true;
                result.push_back(&seat);

                if (result.size() == count)
                    return result;
            }
        }

        return result;
    }

    void releaseSeats(vector<Seat*> seats)
    {
        for (auto s : seats)
            s->occupied = false;
    }

    double calculatePrice(Session* session, Zone zone)
    {
        int hour = stoi(session->time.substr(0, 2));

        double price = session->basePrice;

        if (hour < 12)
            price *= 0.75;
        else if (hour > 18)
            price *= 1.5;

        if (zone == Zone::VIP)
            price *= 2;

        return price;
    }
};

class TicketOffice
{
private:
    Cinema& cinema;

public:
    TicketOffice(Cinema& c) : cinema(c) {}

    vector<Ticket> buyTickets(
        string date,
        string time,
        string movie,
        int hall,
        Zone zone,
        int count,
        double& totalPrice)
    {
        Session* session = cinema.findSession(date, time, movie, hall);

        if (!session)
            throw runtime_error("Session not found");

        if (!cinema.checkSeats(session, zone, count))
            throw runtime_error("Not enough seats");

        auto seats = cinema.reserveSeats(session, zone, count);

        double price = cinema.calculatePrice(session, zone);
        totalPrice = price * count;

        vector<Ticket> tickets;

        for (auto s : seats)
        {
            tickets.push_back({
                date,
                time,
                movie,
                hall,
                s->row,
                s->number
                });
        }

        return tickets;
    }

    void cancelOrder(vector<Seat*> seats)
    {
        cinema.releaseSeats(seats);
    }
};

Session createSession(
    string date,
    string time,
    string movie,
    int hall,
    int rows,
    int seatsPerRow,
    double basePrice)
{
    Session s;

    s.date = date;
    s.time = time;
    s.movie = movie;
    s.hall = hall;
    s.basePrice = basePrice;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < seatsPerRow; c++)
        {
            Seat seat;
            seat.row = r + 1;
            seat.number = c + 1;

            if (r < 2)
                seat.zone = Zone::VIP;
            else
                seat.zone = Zone::Regular;

            s.seats.push_back(seat);
        }
    }

    return s;
}

int main() {};