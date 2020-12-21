#include "../include/steam_controller/steam_controller.hpp"
#include <iostream>

bool check_update(steam_controller::update_event& update)
{
  // End when pressing HOME+B
  if (update.buttons & static_cast<int>(steam_controller::Button::HOME ) &&
	  update.buttons & static_cast<int>(steam_controller::Button::B)     )
    return false;

  if (update.buttons != 0)
	  std::cout << "buttons: 0x" << std::hex << update.buttons << " " << std::dec << std::endl;
  if (update.left_axis.x != 0 || update.left_axis.y != 0)
	  std::cout << "left: (" << update.left_axis.x << "," << update.left_axis.y << ") " << std::endl;
  if (update.right_axis.x != 0 || update.right_axis.y != 0)
	  std::cout << "right: (" << update.right_axis.x << ", " << update.right_axis.y << ") " << std::endl;
  if(update.left_trigger != 0)
	  std::cout << "left trigger: " << update.left_trigger << " " << std::endl;
  if (update.right_trigger != 0)
	  std::cout << "right trigger: " << update.right_trigger << " " << std::endl;

  return true;
}

void try_opening(steam_controller::context& context, steam_controller::connection_info const& info)
{

  auto controller = context.connect(info, 0, std::chrono::milliseconds(500));
  if (!controller)
      return;

  steam_controller::event event{};

  while (true)
  {
    auto state_before = controller->state();

    controller->poll(event);

    auto state = controller->state();
    if (state == steam_controller::connection_state::disconnected)
    {
      // Note: controllers that are never attached will still appear as disconnected.
      // Need to keep searching for one that is connected!
      std::cout << "Controller disconnected\n";
      break;
    }

    if (state == steam_controller::connection_state::connected && state_before != steam_controller::connection_state::connected)
    {
      std::cout << "Controller connected\n";
    }

    if (event.key == steam_controller::event_key::UPDATE)
    {
      if (!check_update(event.update))
        break;
    }
  }
}

int main(int argn, char** argv)
{
  steam_controller::context context;

  auto all = context.enumerate();

  if (all.empty())
  {
    std::cerr << "No controllers found!\n";
    return -1;
  }

  for (auto const& each : all)
  {
    try_opening(context, each);
  }

  return 0;
}
