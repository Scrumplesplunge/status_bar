#include "volume.h"

const char* const MIXER_NAME = "default";
const char* const CONTROLLER = "Master";

CalculateVolume::CalculateVolume(Buffer* buffer, Task* on_update)
    : buffer_(buffer), on_update_(on_update) {
  // Open the mixer.
  snd_mixer_open(&mixer_handle_, 0);
  snd_mixer_attach(mixer_handle_, MIXER_NAME);
  snd_mixer_selem_register(mixer_handle_, nullptr, nullptr);
  snd_mixer_load(mixer_handle_);

  snd_mixer_selem_id_malloc(&element_id_);
  snd_mixer_selem_id_set_name(element_id_, CONTROLLER);

  // Find the device element.
  element_ = snd_mixer_find_selem(mixer_handle_, element_id_);
}

CalculateVolume::~CalculateVolume() {
  if (element_ != nullptr) snd_mixer_elem_free(element_);
  if (element_id_ != nullptr) snd_mixer_selem_id_free(element_id_);
  if (mixer_handle_ != nullptr) snd_mixer_close(mixer_handle_);
}

void CalculateVolume::Perform(Executor* executor) {
  // Fetch the current volume, and the min and max volume bounds.
  long int volume, min_volume, max_volume;
  snd_mixer_handle_events(mixer_handle_);
  snd_mixer_selem_get_playback_volume_range(
      element_, &min_volume, &max_volume);
  snd_mixer_selem_get_playback_volume(
      element_, SND_MIXER_SCHN_UNKNOWN, &volume);
  // Compute the volume.
  double percent = 100.0 * volume / (max_volume - min_volume);
  int rounded_percent = static_cast<int>(percent + 0.5);
  snprintf(buffer_->get(), buffer_->size(), "%d%%", rounded_percent);

  executor->Schedule(on_update_);
}
