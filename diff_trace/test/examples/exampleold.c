void f() {

  a();
  b();
  c() + d();
  e();

}

void g() {}

void h() {

  a(b());
}

void f2() {

  a();
  b();
  c() + d();

}

void h2() {

  d() + a(b()) + e();
}

void h3() {

  d() + a(b() + c()) + e();
}

void h4() {

  d() + a(b(f()) + c()) + e();

  a = b;

  if(a < b)
    c;

  if(a < b)
    c;
}
