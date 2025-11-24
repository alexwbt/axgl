#pragma once

#include <memory>
#include <cstdint>

#include <axgl/common.hpp>
#include <axgl/interface/window.hpp>
#include <utility>

NAMESPACE_AXGL_INTERFACE

enum class InputSource
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
};

enum class PointerSource
{
  kMouseMove,
};

enum class CursorMode
{
  kLocked,
  kNormal,
};

class Input final
{
  static uint32_t next_id()
  {
    static uint32_t next_id_ = 1;
    return next_id_++;
  }

public:
  const uint32_t id;
  std::string name;
  InputSource source;
  uint32_t tick;

  Input(std::string name, const InputSource source) : id(next_id()), name(std::move(name)), source(source), tick(0) { }
};

class Pointer final
{
  static uint32_t next_id()
  {
    static uint32_t next_id_ = 1;
    return next_id_++;
  }

public:
  const uint32_t id;
  std::string name;
  PointerSource source;
  glm::ivec2 position;
  glm::ivec2 delta;
  uint32_t tick;

  Pointer(std::string name, const PointerSource source) :
    id(next_id()), name(std::move(name)), source(source), position(0), delta(0), tick(0)
  {
  }
};

class InputService : virtual public Service
{
public:
  virtual void set_window(std::shared_ptr<Window> window) = 0;
  virtual void add_input(std::shared_ptr<Input> input) = 0;
  virtual void add_pointer(std::shared_ptr<Pointer> pointer) = 0;
  virtual void remove_input(uint32_t id) = 0;
  virtual void remove_pointer(uint32_t id) = 0;

  virtual void set_cursor_mode(CursorMode mode) = 0;

  friend class Input;
  friend class Pointer;
};

NAMESPACE_AXGL_INTERFACE_END
