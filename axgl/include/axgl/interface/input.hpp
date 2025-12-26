#pragma once

#include <cstdint>
#include <string>

namespace axgl
{

class Input final
{
public:
  enum class Source
  {
    kKeySpace,
    kKeyApostrophe,
    kKeyComma,
    kKeyMinus,
    kKeyPeriod,
    kKeySlash,
    kKey0,
    kKey1,
    kKey2,
    kKey3,
    kKey4,
    kKey5,
    kKey6,
    kKey7,
    kKey8,
    kKey9,
    kKeySemicolon,
    kKeyEqual,
    kKeyA,
    kKeyB,
    kKeyC,
    kKeyD,
    kKeyE,
    kKeyF,
    kKeyG,
    kKeyH,
    kKeyI,
    kKeyJ,
    kKeyK,
    kKeyL,
    kKeyM,
    kKeyN,
    kKeyO,
    kKeyP,
    kKeyQ,
    kKeyR,
    kKeyS,
    kKeyT,
    kKeyU,
    kKeyV,
    kKeyW,
    kKeyX,
    kKeyY,
    kKeyZ,
    kKeyLeftBracket,
    kKeyBackslash,
    kKeyRightBracket,
    kKeyGraveAccent,
    kKeyWorld1,
    kKeyWorld2,
    kKeyEscape,
    kKeyEnter,
    kKeyTab,
    kKeyBackspace,
    kKeyInsert,
    kKeyDelete,
    kKeyRight,
    kKeyLeft,
    kKeyDown,
    kKeyUp,
    kKeyPageUp,
    kKeyPageDown,
    kKeyHome,
    kKeyEnd,
    kKeyCapsLock,
    kKeyScrollLock,
    kKeyNumLock,
    kKeyPrintScreen,
    kKeyPause,
    kKeyF1,
    kKeyF2,
    kKeyF3,
    kKeyF4,
    kKeyF5,
    kKeyF6,
    kKeyF7,
    kKeyF8,
    kKeyF9,
    kKeyF10,
    kKeyF11,
    kKeyF12,
    kKeyF13,
    kKeyF14,
    kKeyF15,
    kKeyF16,
    kKeyF17,
    kKeyF18,
    kKeyF19,
    kKeyF20,
    kKeyF21,
    kKeyF22,
    kKeyF23,
    kKeyF24,
    kKeyF25,
    kKeyKeypad0,
    kKeyKeypad1,
    kKeyKeypad2,
    kKeyKeypad3,
    kKeyKeypad4,
    kKeyKeypad5,
    kKeyKeypad6,
    kKeyKeypad7,
    kKeyKeypad8,
    kKeyKeypad9,
    kKeyKeypadDecimal,
    kKeyKeypadDivide,
    kKeyKeypadMultiply,
    kKeyKeypadSubtract,
    kKeyKeypadAdd,
    kKeyKeypadEnter,
    kKeyKeypadEqual,
    kKeyLeftShift,
    kKeyLeftControl,
    kKeyLeftAlt,
    kKeyLeftSuper,
    kKeyRightShift,
    kKeyRightControl,
    kKeyRightAlt,
    kKeyRightSuper,
    kKeyMenu,
    kMouseButton1,
    kMouseButton2,
    kMouseButton3,
    kMouseButton4,
    kMouseButton5,
    kMouseButton6,
    kMouseButton7,
    kMouseButton8,
  };

private:
  static std::uint64_t next_id()
  {
    static std::uint64_t next_id_ = 1;
    return next_id_++;
  }

public:
  const std::uint64_t id;
  std::string name;
  axgl::Input::Source source;
  std::uint64_t tick;

  Input(std::string name, const axgl::Input::Source source) :
    id(next_id()), name(std::move(name)), source(source), tick(0)
  {
  }
};

} // namespace axgl
