#pragma once
#include <stdint.h>
#include "encoder.h"

namespace EncoderPhysicalLinker {
  //! Initialize the encoder controller linker with its interrupt routines
  void initialize(void);

  //! Link a motor encoder to an interrupt routine
  void linkEncoder(TwoPhaseEncoder *enc);

  //! Start the encoder physical linker activity
  void start(void);

  //! Stop the encoder physical linker activity
  void stop(void);
};
