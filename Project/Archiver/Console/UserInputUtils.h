// UserInputUtils.h

#pragma once

#ifndef __USERINPUTUTILS_H
#define __USERINPUTUTILS_H

namespace NUserAnswerMode {

enum EEnum
{
  kYes,
  kNo,
  kYesAll,
  kNoAll,
  kQuit,
};
}

NUserAnswerMode::EEnum ScanUserYesNoAllQuit();

#endif