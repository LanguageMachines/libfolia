#include <iostream>

using namespace std;

class base {
public:
  virtual int getId() { return id; };
private:
  const static int id =1;
};

class inh: public base {
public:
  int getId() { return id; };
private:
  const static int id =3;
};

class base2 {
public:
  static int getId() { return id; };
private:
  static int id;
};

int base2::id =10;

class inh2: public base2 {
public:
  static int getId() { return id; };
private:
  static int id;
};

int inh2::id =30;

class base3 {
public:
  static const int id;
};

const int base3::id =100;

class inh3: public base3 {
public:
  static const int id;
};

const int inh3::id =300;

class basebase {
public:
  virtual int getId() = 0;
private:
};

class base4 : public basebase {
public:
  int getId() { return id; };
  static const int id;
};

const int base4::id =1000;

class inh4: public base4 {
public:
  int getId() { return id; };
  static const int id;
};

const int inh4::id =3000;

main() {
  base B;
  inh I;
  cerr << "Base " << B.getId() << endl;
  cerr << "Inh " << I.getId() << endl;
  base2 B2;
  inh2 I2;
  cerr << "Base2 " << B2.getId() << endl;
  cerr << "Inh2 " << I2.getId() << endl;
  cerr << "zonder een class: " << endl;
  cerr << "Base2 " << base2::getId() << endl;
  cerr << "Inh2 " << inh2::getId() << endl;
  base3 B3;
  inh3 I3;
  cerr << "zonder een class, en zonder functie " << endl;
  cerr << "Base3 " << base3::id << endl;
  cerr << "Inh3 " << inh3::id << endl;
  base4 B4;
  inh4 I4;
  cerr << "nog anders " << endl;
  cerr << "Base4 " << B4.getId() << endl;
  cerr << "Inh4 " << I4.getId() << endl;
  cerr << "Base4 " << base4::id << endl;
  cerr << "Inh4 " << inh4::id << endl;
}
