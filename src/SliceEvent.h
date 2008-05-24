// TODO: do properly

#ifndef SLICE_EVENT_GUARD
#define SLICE_EVENT_GUARD

class SliceEvent {
 public:
  virtual ~SliceEvent() {}

  virtual void raiseEvent() = 0;
};

#endif
