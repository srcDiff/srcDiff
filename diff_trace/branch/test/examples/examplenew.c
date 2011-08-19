void f1() {

  a();
  b();
  c() + d();

}

void g(int a, int b) {}

void h1() {

  a(b());
}

void f2() {

  a();
  b();
  c() + d();
  e();

}

void h2() {

  d() + a(b()) + e();
}

void h3() {

  d() + a(b() + c()) + e();
}

void h4() {

  d() + a(b(f()) + c()) + e();

  if(a < b)
    c;

  if(a > b)
    c;
}
