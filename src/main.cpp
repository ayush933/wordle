#include <cassert>
#include <cctype>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/screen/screen.hpp>
#include <words.hpp>

class Grid
{
  int cur_idx;
  void process()
  {
    std::set<char> colored;
    // 1st pass for Green
    for (int x = 0; x < 5; x++) {
      if (strs[cur_idx][x] == word[x]) {
        clrs[cur_idx][x] = 1;
        colored.insert(strs[cur_idx][x]);
      }
    }
    // 2nd pass for Yellow
    for (int x = 0; x < 5; x++) {
      if (clrs[cur_idx][x] or colored.contains(strs[cur_idx][x])) continue;
      for (int y = 0; y < 5; y++) {
        if (word[y] == strs[cur_idx][x]) {
          clrs[cur_idx][x] = 2;
          colored.insert(strs[cur_idx][x]);
        }
      }
    }
    // 3rd pass for Gray
    for (int x = 0; x < 5; x++) {
      if (!clrs[cur_idx][x]) clrs[cur_idx][x] = 3;
    }
    if (word == strs[cur_idx])
      end = 1;
    else if (cur_idx == 5)
      end = 2;
    else
      cur_idx++;
  }

public:
  int end;
  std::string word;
  std::vector<std::string> strs;
  std::vector<std::vector<int>> clrs;

  Grid()
    : end{ 0 }, word{ rand_str() }, cur_idx{ 0 }, strs{ std::vector<std::string>(6, "") }, clrs{
        std::vector<std::vector<int>>(6, std::vector<int>(5, 0))
      }
  {}

  void addChar(std::string s)
  {
    assert(cur_idx < 6);
    char c = s[0];
    if (!std::isalpha(c) or strs[cur_idx].length() >= 5) return;
    c = toupper(c);
    strs[cur_idx] += c;
  }

  void delChar()
  {
    assert(cur_idx >= 0 and cur_idx < 6);
    if (!strs[cur_idx].length()) return;
    strs[cur_idx].pop_back();
  }

  void enter()
  {
    if (strs[cur_idx].length() < 5) return;
    process();
  }
};


int main()
{
  using namespace ftxui;

  Grid state;
  auto screen = ScreenInteractive::FitComponent();

  auto component = Renderer([&state, &screen] {
    Elements children, row;

    for (int x = 0; x < 6; x++) {
      std::string str = state.strs[x];
      auto back_clr = bgcolor(Color::White);

      for (int y = 0; y < 5; y++) {
        char c = (y < str.length() ? str[y] : ' ');
        auto child = text(std::string(1, c)) | color(Color::White) | bold | center | size(WIDTH, EQUAL, 4);

        if (state.clrs[x][y]) {
          if (state.clrs[x][y] == 1)
            back_clr = bgcolor(Color::DarkGreen);
          else if (state.clrs[x][y] == 2)
            back_clr = bgcolor(Color::DarkGoldenrod);
          else
            back_clr = bgcolor(Color::GrayDark);
          child |= back_clr;
        }

        child |= border;
        row.push_back(child);
      }

      children.push_back(hbox(std::move(row)));
      row.clear();
    }

    if (state.end) {
      std::string msg;
      auto txt_clr = color(Color::White);
      if (state.end == 1)
        msg = "GOOD JOB!!YOU WIN";
      else
        msg = "GAME OVER.YOU LOSE";
      children.push_back(separator());
      auto child = text(msg) | bold | center;
      if (state.end == 1)
        txt_clr = color(Color::Green);
      else
        txt_clr = color(Color::Red);
      child |= txt_clr;
      children.push_back(child);
      if (state.end == 2) children.push_back(text("ANSWER=" + state.word) | bold | center | color(Color::Magenta));
      screen.ExitLoopClosure()();
    }

    return window(text("WORDLE") | center, vbox(std::move(children))) | color(Color::Magenta);
  });

  component = CatchEvent(component, [&](Event event) {
    if (event == Event::Escape)
      screen.ExitLoopClosure()();
    else if (event == Event::Special({ 10 }))
      state.enter();
    else if (event == Event::Backspace)
      state.delChar();
    else
      state.addChar(event.input());
    return false;
  });

  screen.Loop(component);

  return EXIT_SUCCESS;
}