#include "jcanvas/core/japplication.h"
#include "jcanvas/core/jwindow.h"
#include "jcanvas/core/jindexedimage.h"
#include "jcanvas/core/jbufferedimage.h"

using namespace jcanvas;

struct Doom : public Window, public MouseListener, public WindowListener {
  Doom(): Window(jpoint_t<int>{720, 480}) {
    SetFramesPerSecond(24);
    RegisterWindowListener(this);
  }

  ~Doom() override {
  }

  void Paint(Graphics *g) override {
    Window::Paint(g);

    auto size = GetSize();
    auto vLine = jline_t<int>{{size.x / 2, 0}, {size.x / 2, size.y}} + mOffset;
    auto hLine = jline_t<int>{{0, size.y / 2}, {size.x, size.y / 2}} + mOffset;

    vLine.p0.y = std::min(0, vLine.p0.y);
    vLine.p1.y = std::max(size.y, vLine.p1.y);

    hLine.p0.x = std::min(0, hLine.p0.x);
    hLine.p1.x = std::max(size.x, hLine.p1.x);

    g->SetColor(jcolor_name_t::White);
    g->DrawLine(vLine.p0, vLine.p1);
    g->DrawLine(hLine.p0, hLine.p1);
  }

  bool MousePressed([[maybe_unused]] MouseEvent *event) override {
    mPressed = true;

    return true;
  }

  bool MouseReleased([[maybe_unused]] MouseEvent *) override {
    mPressed = false;

    return true;
  }

  bool MouseMoved([[maybe_unused]] MouseEvent *event) override {
    auto diffLocation = event->GetLocation() - mLastLocation;

    mLastLocation = event->GetLocation();

    if (mFirst) {
      mFirst = false;

      return true;
    }

    if (mPressed == false) {
      return true;
    }

    mOffset = mOffset + diffLocation;

    std::cout << "L: " << mOffset.x << ", " << mOffset.y << std::endl;

    Repaint();

    return true;
  }

  void WindowOpened([[maybe_unused]] WindowEvent *event) override {
  }

  void WindowClosing([[maybe_unused]] WindowEvent *event) override {
  }

  void WindowClosed([[maybe_unused]] WindowEvent *event) override {
  }

  void WindowResized([[maybe_unused]] WindowEvent *event) override {
    Repaint();
  }

  void WindowMoved([[maybe_unused]] WindowEvent *event) override {
  }

  void WindowPainted([[maybe_unused]] WindowEvent *event) override {
  }

  void WindowEntered([[maybe_unused]] WindowEvent *event) override {
  }

  void WindowLeaved([[maybe_unused]] WindowEvent *event) override {
  }

private:
  jpoint_t<int> mOffset{0, 0};
  jpoint_t<int> mLastLocation{0, 0};
  bool mFirst{true};
  bool mPressed{false};
};

int main(int argc, char *argv[]) {
  Application::Init(argc, argv);

  Doom app;

  app.SetTitle("Doom");
  app.Exec();

  Application::Loop();

  return 0;
}
