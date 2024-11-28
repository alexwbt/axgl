#pragma once

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
      virtual ~IteratorWrapper() {}

      virtual reference current() const = 0;
      virtual bool equal(const IteratorWrapper* rhs) const = 0;

      virtual void next() = 0;
      virtual void assign(const IteratorWrapper* rhs) = 0;

      virtual IteratorWrapper* clone() = 0;
    };

    template<typename Iterator>
    class Adaptor : public IteratorWrapper
    {
    private:
      Iterator iterator_;

    public:
      Adaptor(const Iterator& rhs) : iterator_(rhs) {}
      Adaptor(const Adaptor& other) : iterator_(other.iterator_) {}

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

      IteratorWrapper* clone() override
      {
        return new Adaptor<Iterator>(iterator_);
      }
    };

  private:
    IteratorWrapper* wrapper_;

  public:
    AnyIterator() : wrapper_(nullptr) {}
    AnyIterator(IteratorWrapper* wrapper) : wrapper_(wrapper) {}
    AnyIterator(const AnyIterator& other)
    {
      if (other.wrapper_)
        wrapper_ = other.wrapper_->clone();
      else
        wrapper_ = nullptr;
    }
    AnyIterator(AnyIterator&& other)
    {
      wrapper_ = other.wrapper_;
      other.wrapper_ = nullptr;
    }
    ~AnyIterator()
    {
      if (wrapper_)
        delete wrapper_;
    }

    AnyIterator& operator++() { wrapper_->next(); return *this; }
    reference operator* () const { return wrapper_->current(); }
    pointer operator->() const { return &wrapper_->current(); }
    bool operator==(const AnyIterator& rhs) const { return wrapper_->equal(rhs.wrapper_); }
    bool operator!=(const AnyIterator& rhs) const { return !wrapper_->equal(rhs.wrapper_); }
  };

  template <typename ValueType>
  class Iterable
  {
  private:
    AnyIterator<ValueType> begin_;
    AnyIterator<ValueType> end_;

  public:
    Iterable(AnyIterator<ValueType> begin, AnyIterator<ValueType> end)
      : begin_(std::move(begin)), end_(std::move(end))
    {}

    Iterable(const Iterable& other)
      : begin_(other.begin_), end_(other.end_)
    {}

    Iterable(Iterable&& other)
      : begin_(std::move(other.begin_)), end_(std::move(other.end_))
    {}

    AnyIterator<ValueType> begin() const
    {
      return begin_;
    }

    AnyIterator<ValueType> end() const
    {
      return end_;
    }
  };

  template <typename ValueType, typename ContainerType>
  inline Iterable<ValueType> to_iterable(const ContainerType& data)
  {
    return {
      util::AnyIterator<ValueType>(new util::AnyIterator<ValueType>::Adaptor(std::begin(data))),
      util::AnyIterator<ValueType>(new util::AnyIterator<ValueType>::Adaptor(std::end(data)))
    };
  }

}
