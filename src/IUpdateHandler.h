#pragma once

class IUpdateHandler
{
public:
  virtual ~IUpdateHandler() = default;
  virtual void Update() = 0;
};