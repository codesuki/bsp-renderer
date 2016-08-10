#ifndef PLAYER_ANIMATION_COMPONENT_HPP_
#define PLAYER_ANIMATION_COMPONENT_HPP_

#include "component.hpp"
#include "model.hpp"

class PlayerAnimationComponent : public Component {
public:
  PlayerAnimationComponent();
  ~PlayerAnimationComponent();

  void ReceiveMessage(Message &message){};
  void Update(unsigned int time);

private:
  int upper_current_animation_;
  int lower_current_animation_;
  PlayerModel model_;
};

#endif
