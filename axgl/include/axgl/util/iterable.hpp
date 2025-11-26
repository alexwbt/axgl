#pragma once

#include <memory>

namespace util
{

template <typename ValueType>
class AnyIterator final
{
public:
  using iterator_category = std::input_iterator_tag;
  using value_type = ValueType;
  using difference_type = std::ptrdiff_t;
  using pointer = const ValueType*;
  using reference = const ValueType&;

  class IteratorWrapper
  {
  public:
    virtual ~IteratorWrapper() = default;

    virtual reference current() const = 0;
    virtual bool equal(const IteratorWrapper* rhs) const = 0;

    virtual void next() = 0;
    virtual void assign(const IteratorWrapper* rhs) = 0;

    virtual std::unique_ptr<IteratorWrapper> clone() const = 0;
  };

  template <typename Iterator>
  class Adaptor : public IteratorWrapper
  {
    Iterator iterator_;

  public:
    explicit Adaptor(const Iterator& rhs) : iterator_(rhs) { }
    Adaptor(const Adaptor& other) : iterator_(other.iterator_) { }

    reference current() const override { return *iterator_; }
    bool equal(const IteratorWrapper* rhs) const override
    {
      return iterator_ == static_cast<const Adaptor<Iterator>*>(rhs)->iterator_;
    }

    void next() override { ++iterator_; }
    void assign(const IteratorWrapper* rhs) override
    {
      iterator_ = static_cast<const Adaptor<Iterator>*>(rhs)->iterator_;
    }

    std::unique_ptr<IteratorWrapper> clone() const override { return std::make_unique<Adaptor<Iterator>>(iterator_); }
  };

private:
  std::unique_ptr<IteratorWrapper> wrapper_;

public:
  AnyIterator() : wrapper_(nullptr) { }
  explicit AnyIterator(std::unique_ptr<IteratorWrapper> wrapper) : wrapper_(std::move(wrapper)) { }
  AnyIterator(const AnyIterator& other)
  {
    if (other.wrapper_)
      wrapper_ = other.wrapper_->clone();
    else
      wrapper_ = nullptr;
  }

  AnyIterator& operator++()
  {
    wrapper_->next();
    return *this;
  }
  reference operator*() const { return wrapper_->current(); }
  pointer operator->() const { return &wrapper_->current(); }
  bool operator==(const AnyIterator& rhs) const { return wrapper_->equal(rhs.wrapper_.get()); }
  bool operator!=(const AnyIterator& rhs) const { return !(*this == rhs); }
};

template <typename ValueType>
class Iterable
{
  AnyIterator<ValueType> begin_;
  AnyIterator<ValueType> end_;

public:
  Iterable(AnyIterator<ValueType> begin, AnyIterator<ValueType> end) :
    begin_(std::move(begin)), end_(std::move(end)) { }

  Iterable(const Iterable& other) : begin_(other.begin_), end_(other.end_) { }

  Iterable(Iterable&& other) noexcept : begin_(std::move(other.begin_)), end_(std::move(other.end_)) { }

  AnyIterator<ValueType> begin() const { return begin_; }

  AnyIterator<ValueType> end() const { return end_; }
};

template <typename ValueType, typename ContainerType>
Iterable<ValueType> to_iterable_t(const ContainerType& data)
{
  using WrapperType = typename AnyIterator<ValueType>::template Adaptor<decltype(std::begin(data))>;
  return {AnyIterator<ValueType>(std::make_unique<WrapperType>(std::begin(data))),
    AnyIterator<ValueType>(std::make_unique<WrapperType>(std::end(data)))};
}

template <typename ContainerType>
auto to_iterable(const ContainerType& data) -> Iterable<typename std::decay<decltype(*std::begin(data))>::type>
{
  using ValueType = typename std::decay<decltype(*std::begin(data))>::type;
  return to_iterable_t<ValueType>(data);
}

} // namespace util
