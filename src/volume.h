#pragma once

#include "buffer.h"
#include "config.h"
#include "executor.h"

#include <alsa/asoundlib.h>

class CalculateVolume : public Task {
 public:
  CalculateVolume(const Config& config, Buffer* buffer, Task* on_update);
  ~CalculateVolume() override;

 private:
  void Perform(Executor* executor) override;

  Buffer* buffer_;
  Task* on_update_;

  snd_mixer_t* mixer_handle_ = nullptr;
  snd_mixer_selem_id_t* element_id_ = nullptr;
  snd_mixer_elem_t* element_ = nullptr;

  int volume_rounding_unit_;
};
