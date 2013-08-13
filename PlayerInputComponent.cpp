#include "PlayerInputComponent.h"
#include "messenger.h"

PlayerInputComponent::PlayerInputComponent(Entity& entity) : entity_(entity)
{
  //messenger::RegisterReceiver(MESSAGE::CMD_MOUSELOOK, &PlayerInputComponent::MouseMove);
}


PlayerInputComponent::~PlayerInputComponent(void)
{
}
