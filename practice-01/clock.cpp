#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Message {
public:
  int lamport;
  vector<int> vc;
};

class Event {
public:
  string name;
  int lamport;
  vector<int> vc;
};

vector<Event> events;

class Process {
public:
  int id;
  int lamport;
  vector<int> vc;

  Process(int id, int n) {
    this->id = id;
    lamport = 0;
    vc = vector<int>(n, 0);
  }

  void local(string name) {
    lamport++;
    vc[id]++;
    save(name);
  }

  Message send(string name) {
    lamport++;
    vc[id]++;
    save(name);
    Message m;
    m.lamport = lamport;
    m.vc = vc;
    return m;
  }

  void receive(string name, Message m) {
    lamport = max(lamport, m.lamport) + 1;
    for (int k = 0; k < (int)vc.size(); k++)
      vc[k] = max(vc[k], m.vc[k]);
    vc[id]++;
    save(name);
  }

private:
  void save(string name) {
    Event e;
    e.name = name;
    e.lamport = lamport;
    e.vc = vc;
    events.push_back(e);
  }
};

string vecToStr(vector<int> v) {
  string s = "(";
  for (int i = 0; i < (int)v.size(); i++) {
    s += to_string(v[i]);
    if (i + 1 < (int)v.size())
      s += ",";
  }
  return s + ")";
}

Event find(string name) {
  for (Event e : events)
    if (e.name == name)
      return e;
  return events[0];
}

bool lessEq(vector<int> a, vector<int> b) {
  for (int i = 0; i < (int)a.size(); i++)
    if (a[i] > b[i])
      return false;
  return true;
}

void compareLamport(Event a, Event b) {
  cout << a.name << " (l=" << a.lamport << ") и " << b.name
       << " (l=" << b.lamport << "): ";
  if (a.lamport < b.lamport)
    cout << a.name << " < " << b.name;
  else if (a.lamport > b.lamport)
    cout << a.name << " > " << b.name;
  else
    cout << "метки равны";
  cout << endl;
}

void compareVector(Event a, Event b) {
  cout << a.name << " " << vecToStr(a.vc) << " и " << b.name << " "
       << vecToStr(b.vc) << ": ";
  if (a.vc == b.vc)
    cout << "одно и то же событие";
  else if (lessEq(a.vc, b.vc))
    cout << a.name << " -> " << b.name << " (произошло раньше)";
  else if (lessEq(b.vc, a.vc))
    cout << b.name << " -> " << a.name << " (произошло раньше)";
  else
    cout << a.name << " || " << b.name << " (параллельные)";
  cout << endl;
}

int main() {
  Process p1(0, 3), p2(1, 3), p3(2, 3);

  p1.local("e1");
  p1.local("e2");
  Message m1 = p1.send("e3");
  Message m2 = p3.send("e5");
  p2.receive("e8", m2);
  p2.receive("e4", m1);
  Message m3 = p3.send("e6");
  p1.receive("e7", m3);

  cout << "метки событий" << endl;
  for (Event e : events)
    cout << e.name << ": лэмпорт = " << e.lamport
         << ", вектор = " << vecToStr(e.vc) << endl;

  vector<pair<string, string>> pairs = {
      {"e3", "e4"}, {"e1", "e7"}, {"e5", "e2"}, {"e6", "e3"}};

  cout << endl << "а) сравнение по счётчику событий (лэмпорт)" << endl;
  for (auto p : pairs)
    compareLamport(find(p.first), find(p.second));

  cout << endl << "б) сравнение по векторным часам" << endl;
  for (auto p : pairs)
    compareVector(find(p.first), find(p.second));

  return 0;
}
