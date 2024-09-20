# X-ellent User Manual

## Configuration

X-ellent can be configured using either X resources or a local configuration file (`xellent.conf`).

### Using X Resources

Set X resources in your `.Xresources` file:

```
xellent.color.white: white
xellent.color.black: black
xellent.color.red: red
xellent.color.blue: DodgerBlue
xellent.color.yellow: yellow
xellent.color.green: SeaGreen
xellent.color.grey: grey60
xellent.color.ice: white
xellent.color.termhi: green1
xellent.color.termlo: green3
```

### Using Local Configuration File

Create a file named `xellent.conf` in the same directory as the X-ellent executable:

```
color.white=white
color.black=black
color.red=red
color.blue=DodgerBlue
color.yellow=yellow
color.green=SeaGreen
color.grey=grey60
color.ice=white
color.termhi=green1
color.termlo=green3
```

### Key Mappings

Key mappings can be customized in the `xellent.conf` file:

```
key.rotate_left=X
key.rotate_right=Z
key.thrust=Shift_L
key.brake=space
key.fire=Return
key.change_thrust_up=bracketright
key.change_thrust_down=bracketleft
key.change_spin_up=apostrophe
key.change_spin_down=semicolon
key.change_target=T
key.radar_range_up=equal
key.radar_range_down=minus
key.radar_scan_up=R
key.radar_scan_current=G
key.radar_scan_down=V
key.change_weapon=C
key.enter_shop=E
key.buy_fuel=F
key.bind_object=B
key.drop_mine=Tab
key.arm_mine=A
key.disarm_mine=U
key.detach_mine=D
key.change_mine_settings_up=period
key.change_mine_settings_down=slash
key.toggle_status=S
key.toggle_weapon_slots=W
key.toggle_messages=M
key.toggle_hud=Delete
key.toggle_iff=N
key.toggle_invisibility=I
key.toggle_cloak=O
key.toggle_anticloak=Y
key.toggle_minesweeper=P
key.leave_game=H
key.summon_lift=L
key.lift_up=Up
key.lift_down=Down
```

## Connecting to the Game

1. Ensure the X-ellent server is running.
2. Run the client: `./myclient [server_address] [password]`
3. The client will connect to the server and send your configuration.

## Frontend Functions

Function keys (F1 to F12 and KP_F1 to KP_F4) can be used to trigger frontend functions when attached.

