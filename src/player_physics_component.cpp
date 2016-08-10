#include "player_physics_component.hpp"

#include "bsp.hpp"
#include "entity.hpp"
#include "input.hpp"
#include "logger.hpp"
#include "world.hpp"

// TODO: get rid of those globals (pass to method)
extern World world;
extern cmd_t g_cmds;

PlayerPhysicsComponent::PlayerPhysicsComponent() {}

PlayerPhysicsComponent::~PlayerPhysicsComponent() {}

// TODO: should be in input component
// TODO: remove magic numbers
void PlayerPhysicsComponent::UpdatePitch() {
  auto &pitch = entity_->pitch_;

  logger::Log(logger::DEBUG, "cmd dy: %f", g_cmds.mouse_dy);

  pitch += g_cmds.mouse_dy;
  if (pitch > 1.5707f) {
    pitch = 1.5707f;
  }
  if (pitch < -1.5707f) {
    pitch = -1.5707f;
  }
}

void PlayerPhysicsComponent::UpdateYaw() {
  auto &yaw = entity_->yaw_;

  logger::Log(logger::DEBUG, "cmd dx: %f", g_cmds.mouse_dx);

  yaw += g_cmds.mouse_dx;
  if (yaw > 2 * 3.1415f) {
    yaw = 0.0f;
  }
  if (yaw < 2 * -3.1415f) {
    yaw = 0.0f;
  }
}

#define MIN_WALK_NORMAL 0.7f // can't walk on very steep slopes
#define STEPSIZE 18
#define JUMP_VELOCITY 270
#define OVERCLIP 1.001f
#define GRAVITY 800.0f

void ClipVelocity(glm::vec4 &in, glm::vec4 &out, glm::vec4 &plane) {
  // This should only clip if the velocity goes against the plane
  logger::Debug("clip plane: %f %f %f %f", plane.x, plane.y, plane.z, plane.w);
  logger::Debug("clip velocity: %f %f %f %f", in.x, in.y, in.z, in.w);
  float distance = glm::dot(glm::vec3(plane), glm::vec3(in));
  logger::Debug("clip distance: %f", distance);

  if (distance < 0.0f) {
    distance *= 1.001f;
  } else {
    distance /= 1.001f;
  }

  out = in - distance * plane;
}

/*
 * Note: quake 'time' = pml.frametime
 * pml.frametime = pml.msec * 0.001;
 * msec = 66; when pmove (fixed frame time)
 * 1000/66 = 15 => 15 updates per second? seems low
 */

void PlayerPhysicsComponent::Update(unsigned int time) {
  if (!is_active_) {
    return;
  }

  trace_info trace;

  /*
  auto currentSpeed = glm::length(glm::vec3(entity_->velocity_));
  if (currentSpeed < 1) {
    entity_->velocity_.x = 0;
    entity_->velocity_.y = 0;
    logger::Debug("======= current speed %f smaller than ========",
                  currentSpeed);
  }
  */

  // is this actually correct?? should be applied to right and look vectors
  // because not it seems like they lag a frame behind
  UpdatePitch();
  UpdateYaw();

  float dt = static_cast<float>(time) * 0.001f;

  // GroundTrace();
  // trace to ground and get plane
  glm::vec4 start = entity_->position_;
  glm::vec4 ground_plane;
  glm::vec4 ground = start;
  ground.z -= 0.25f; // 0.25f is from q3 source

  logger::Log(logger::DEBUG, "coords: %f %f %f %f", start.x, start.y, start.z,
              start.w);

  float ground_fraction = world.map_->trace(start, ground, trace);
  ground_plane = trace.plane;

  logger::Debug("ground plane trace results: %d %d %f", trace.starts_out,
                trace.all_solid, trace.fraction);

  logger::Debug("ground plane frac x y z: %f %f %f %f %f", ground_fraction,
                ground_plane.x, ground_plane.y, ground_plane.z, ground_plane.w);

  bool on_ground = false;
  // we hit ground
  if (ground_fraction < 1.0f) {
    entity_->velocity_.z = 0.0f;
    on_ground = true;
    logger::Log(logger::DEBUG, "HIT GROUND");
  }

  logger::Debug("on ground: %d", on_ground);

  float speed = 0.1f;
  // speed = 1.0f;

  // ignore movement in Z direction or else we start flying
  // - maybe set z 0
  // - clip to ground plane
  // - renormalize
  // but not in spectator mode...

  glm::vec4 right = entity_->right_;

  logger::Log(logger::DEBUG, "right: %f %f %f %f", right.x, right.y, right.z,
              right.w);

  glm::vec4 look = entity_->look_;

  logger::Log(logger::DEBUG, "look: %f %f %f %f", look.x, look.y, look.z,
              look.w);

  // if we don't multiply the speed here then it's the direction
  glm::vec4 accel;
  accel += right * speed * static_cast<float>(g_cmds.right_move);
  accel += look * speed * static_cast<float>(g_cmds.forward_move);

  logger::Log(logger::DEBUG, "accel: %f %f %f %f", accel.x, accel.y, accel.z,
              accel.w);

  // accel.x = speed * static_cast<float>(g_cmds.right_move);
  // accel.y = speed * static_cast<float>(g_cmds.forward_move);

  // Accelerate();

  float fraction = 0.0f;

  if (entity_->noclip_) {
    // set boolean for jump key reset
    if (g_cmds.up_move > 0) {
      accel.z = JUMP_VELOCITY;
    }
    // NoClipMove();
    fraction = 1.0f;
    entity_->velocity_ += (accel - 5.0f * entity_->velocity_) * dt;
    entity_->position_ += entity_->velocity_ * fraction;
  }
  // else if (!on_ground)
  //{
  //  //AirMove();
  //  //GroundMove();
  //}
  else {

    right.z = 0.0;
    right = glm::normalize(right);
    look.z = 0.0;
    look = glm::normalize(look);
    accel.z = 0.0f;

    // GroundMove();

    //  accel.z -= 10;

    // maybe we have to remove the ground-hitting velocity already?
    glm::vec4 wish_velocity =
        entity_->velocity_ + (accel - 6.0f * entity_->velocity_) * dt;

    // set boolean for jump key reset
    if (g_cmds.up_move > 0 && on_ground) {
      wish_velocity.z = JUMP_VELOCITY - 250;
      ground_plane = glm::vec4();
    }

    wish_velocity.z = (wish_velocity.z + (wish_velocity.z - 800 * dt)) * 0.5;

    if (wish_velocity.x == 0.0 && wish_velocity.y == 0.0) {
      return;
    }

    ClipVelocity(wish_velocity, wish_velocity, ground_plane);

    glm::vec4 wish_velocity_2 = wish_velocity;

    logger::Debug("wish velocity before clip: %f %f %f %f", wish_velocity.x,
                  wish_velocity.y, wish_velocity.z, wish_velocity.w);

    glm::vec4 wish_position;

    auto bump_count = 0u;
    for (; bump_count < 4; ++bump_count) {
      wish_position = entity_->position_ + wish_velocity;

      fraction = world.map_->trace(entity_->position_, wish_position, trace);

      logger::Debug("::: trace result ::: %d %d", trace.starts_out,
                    trace.all_solid);

      if (trace.all_solid) {
        wish_velocity.z = 0.0;
        ++bump_count;
        break;
      }

      logger::Debug("fraction: %f", fraction);

      if (fraction == 1.0f) {
        break;
      }

      ClipVelocity(wish_velocity, wish_velocity, trace.plane);
      logger::Debug("wish velocity after clip: %f %f %f %f", wish_velocity.x,
                    wish_velocity.y, wish_velocity.z, wish_velocity.w);
    }

    if (!bump_count) {
      entity_->velocity_ = wish_velocity;
      entity_->position_ = entity_->position_ + entity_->velocity_;
      return;
    }

    if (wish_velocity.z > 0.0) {
      logger::Debug("+++++++++++++ WE HAVE UPWARDS VELOCITY +++++++++++++");
    }

    {

      auto down = start;
      down.z -= STEPSIZE;

      glm::vec4 up(0.0, 0.0, 1.0, 0.0);

      fraction = world.map_->trace(start, down, trace);

      auto mydot = glm::dot(trace.plane, up);

      logger::Debug("+++++++++++++ SHOULD WE LEAVE? %f %f  +++++++++++++",
                    fraction, mydot);

      if (wish_velocity.z > 0.0 && (fraction == 1.0 || mydot < 0.7)) {
        logger::Debug("+++++++++++++ LEAVE NOW  +++++++++++++");
        return;
      }
    }

    if (bump_count > 0) {
      // try stepping up
      logger::Debug("============ stepping up =============");
      // trace up to see if there is a ceiling
      // if so we have to limit upwards motion until the ceiling
      // for now just pretend we can always go up a step
      entity_->position_.z += STEPSIZE;

      auto bump_count = 0u;
      for (; bump_count < 4; ++bump_count) {
        wish_position = entity_->position_ + wish_velocity_2;

        logger::Debug("[111] tracing from %f %f %f to %f %f %f",
                      entity_->position_.x, entity_->position_.y,
                      entity_->position_.z, wish_position.x, wish_position.y,
                      wish_position.z);
        fraction = world.map_->trace(entity_->position_, wish_position, trace);

        logger::Debug("::: trace result 2 ::: %d %d", trace.starts_out,
                      trace.all_solid);

        if (trace.all_solid) {
          wish_velocity_2.z = 0.0;
          break;
        }

        logger::Debug("step plane: %f %f %f %f", trace.plane.x, trace.plane.y,
                      trace.plane.z, trace.plane.w);
        logger::Debug("fraction: %f", fraction);

        if (fraction == 1.0f) {
          break;
        }

        ClipVelocity(wish_velocity_2, wish_velocity_2, trace.plane);
        logger::Debug("step wish velocity after clip: %f %f %f %f",
                      wish_velocity_2.x, wish_velocity_2.y, wish_velocity_2.z,
                      wish_velocity_2.w);
      }

      logger::Debug("step final wish velocity after clip: %f %f %f %f",
                    wish_velocity_2.x, wish_velocity_2.y, wish_velocity_2.z,
                    wish_velocity_2.w);

      // trace down from new position to find how high we really need to step
      // needed to avoid stepping up into the air
      entity_->velocity_ = wish_velocity_2;
      entity_->position_ = entity_->position_ + entity_->velocity_;

      glm::vec4 down = entity_->position_;
      down.z -= STEPSIZE;
      // TODO: trace should return the final position to make calculation easier
      // TODO: PRINT FRACTION why are they only 1 and 0
      logger::Debug("tracing from %f %f %f to %f %f %f", entity_->position_.x,
                    entity_->position_.y, entity_->position_.z, down.x, down.y,
                    down.z);

      fraction = world.map_->trace(entity_->position_, down, trace);

      logger::Debug("step down trace plane: %f %f %f %f", trace.plane.x,
                    trace.plane.y, trace.plane.z, trace.plane.w);
      logger::Debug("step down trace fraction startout allsolid: %f %d %d",
                    fraction, trace.starts_out, trace.all_solid);

      entity_->position_ =
          entity_->position_ + fraction * (down - entity_->position_);

      if (fraction < 1.0) {
        ClipVelocity(entity_->velocity_, entity_->velocity_, trace.plane);
        logger::Debug("step down trace  velocity after clip: %f %f %f %f",
                      entity_->velocity_.x, entity_->velocity_.y,
                      entity_->velocity_.z, entity_->velocity_.w);
      }
      logger::Debug("++++ position after second slide move +++ %f %f %f %f +++",
                    entity_->position_.x, entity_->position_.y,
                    entity_->position_.z, entity_->position_.w);

      //      exit(-1);
    }
  }
}

// int NoClipMove()
//{
//}
//
// int GroundMove()
//{
//}

// int GroundTrace()
//{
//// trace to ground and get plane
// glm::vec4 start = position_;
// glm::vec4 ground_plane;
// glm::vec4 ground = start;
// ground.z -= 10.0f - 0.25f;

// float ground_fraction = world.map_->trace(start, ground, &ground_plane);

// bool on_ground = false;
// if (ground_fraction < 1.0f)   // we hit ground
//{
//  on_ground = true;
//}
//}
//
// int SlideMove(glm::vec4 accel)
//{
// accel.z += -9.8f;

// glm::vec4 wish_position;
// glm::vec4 plane;

// unsigned int bump_count;
// for (bump_count = 0; bump_count < 4; ++bump_count)
//{
//  wish_position = position_ + accel;
//  fraction = world.map_->trace(start, wish_position, &plane);
//  if (fraction == 1.0f)
//  {
//    break;
//  }

//  float distance = glm::dot(plane, accel);
//  accel = accel - plane * distance;

//  wish_position = position_ + accel;
//}
//
//
// int SlideMoveStep()
//{
//  int bumps = SlideMove();
//
//  if (bumps == 0)
//  {
//    return;
//  }
//
//  // step up stepsize
//  // trace / slidemove
//}
