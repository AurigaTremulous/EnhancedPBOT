AUTHOR
------
Hi, I'm the author of EPBOTS2.8, my name is Germán Andrés Schwartz.
I'm argentinian but now living in Venezuela.
My email is: lagunaloire@argentina.com or anyeos@cieloaragua.sytes.net
I have a wp: anyeos.wordpress.com (spanish)

FIRST NOTES
-----------
I did not wrote anything before because this mod was made for my own LAN fun.
I will do the best writing here a summary of what I modified/intended.

INTRODUCTION
------------
It is a server side modification only. So you will not need a modified client.
You can play this game with the standard tremulous client. However it is 
recommended to have the version what supports libcurl for faster automatic 
custom maps download.

ABOUT THE NAME
--------------
It is called EPBOTS because I based it in the work of PBOTS. Thank guys for
create tremulous and too for the guys of PBOTS.
The "E" comes from Enhanced, that is all.


HISTORY
-------
It is intended to be a strategic humans vs computer mod.
The aliens are just invading some planets where the humans have some buildings.
Sometimes the humans just meet the aliens in another places not just in where they
have the buildings.

The humans have a technology based weapons and ingeneering to defend against aliens.
The aliens have they claws and some shooting things too but the most powerful
weapon what they have is massive colony of the entire planet. Be careful about that.

Basically it is still tremulous but all weapons, items, equipments, and gameplay
are modified to meet the requeriments of humans vs computer aliens.

HOW TO PLAY/INSTALL
-------------------
First you will need to have a computer where to put the server. I don't will explain
you the steps of how to do it. You can get information about it in internet.
Sorry but I don't know how to compile it in Windows. But in Linux you can go to the
directory "Source" and run make. If you have all the necesary (development 
libraries and tools) you will get the binary files.
Copy the gamex86.so in the base folder of your tremulous server.
Remember what for use the gamex86.so you will need to set "vm_game 0" if not 
the game will try to use only the qvm file.
WARNING: There are problems trying to compile the qvm file. I guess it is impossible
to compile the qvm file because some functions are externals. I guess the form of how
a qvm works does not let to do that. Sorry. Although still there are hope: you can 
try to understand and solve the problem. I tried without success (please tell me if 
you get it).
NOTE: If your server does not start or have trouble you can try the tremded.x86 
obtained in the compilation. That may happen because there are some server fixes 
too and may differ from the original.

Now you must start your server with:
/path/to/your/binary/tremded.x86 +set vm_game 0 +set dedicated 1 +exec server.cfg
for a LAN party and

/path/to/your/binary/tremded.x86 +set vm_game 0 +set dedicated 2 +set net_ip yourinternetip +exec server.cfg
for an internet server

Ready, you now can join the game in another computer.


IMPROVEMENTS/MODIFICATIONS/ADDITIONS
------------------------------------
First of all sorry but the game is translated to spanish because that is my mother tongue.
All the relevant server messages are translated, the rest not.
Anyway this server only shows you some messages, it is not detailed as the original one
because that will lose the objective of this mod: An alien invasion.

You can only join in the humans team. Of course you too can have fun enabling joining
in the aliens team. But just for fun because the game is intended to be a serious some strategic
with action gameplay.
You must defend against aliens. But for that you have a lot of weapons.
I will not explain all details here I just will give you a summary.
The aliens know how to build a lot of structures so you will have a nice challenge.
You will take care about your base and yourself (of course the game is intended to give
support from other players and humans bots too). So you must work in team to get the victory.

The difficult is balanced not too hard but enought. Anyway you can tune it setting the vars.
I just included some example files, there are not too hard to understand but you must pay
attention to fine tunning the things.

You can use "mapconfigs" directory to specify per map configurations.
The most relevant var is g_ambush what determines the time in seconds when the aliens evolve.
When that seconds are reached the aliens (all) evolve to next evolution in next reborn.
With the examples you will understand.

A lot of improvements or let me say, additions, was made. That is because I just wanted
another kind of gameplay.

You will be surprised with the bsuit because it is too slow. But that is compensated with the
extreme resistance that it will give you (at least 60% less damage). And a long jump function,
a onboard batpack and so :P I will let you to discover the rest additions to the battle suit :P

All alien weapons are improved and give you more damage, specially buildings like acid tube 
and hive. Be careful with that.

The alien base is too strong for compensate the brainless of the aliens and to give more
challenge.

So, in summary aliens are strong creatures who wants to kill you at all.

All bots know how to travel in maps but for that they need paths. I will provide you with
some paths but you can do that for yourself with the \node command. There are explanations
about that in the PBOT documentation.

As you can see, it mod is taken from an idea of merging ambush mod and invasion mod together.
Of course, I just rewrited a lot of code for that (and some bugfixes). It is not the same 
as the original ones.

The bots are some more smart and they aim smooth (instead to be instantaneous) so that give
you the impression what they are using a mouse and a keyboard like you. So the bots don't cheat
at all :P

The aliens bots know near all about aliens weapons, spaces, paths and how to kill humans and
destroy bases. They too know how to protect grangers and the grangers are not idiot, they
know how to go away and build. Really, they are some strong and brute not so smart but smart
enought to be a challenge for you. Anyway you can set the skill (reaction) of bots to adjust
the difficult.

The humans know how to follow. They can have a personality file where they get the attributes
preferred weapons, preferred way and so (the bot can have GUID too).
The humans too know how to use the weapons, where and when drop a grenade and so on. They too
remember the way where the action is taking place so they can go quickly to that place while
you explore the rest of the map and they make a contention zone (that is what intended).

The aliens too remember the place of telenodes to destroy it all. And too remember the way
to the action zone.

All bots know how to take care about teammates and buildings to don't hurt the wrong things.


WEAPONS
-------

All weapons are modified (I don't remember if I forget to modify one :P).
Including the blaster. The blaster now is improved depending what kind of 
primary weapon are you carrying. Generally it is improved only if you carry
energy weapons. The same with the battery pack, it gives to the blaster a 
fast shooting rate.
The most significant weapon modification maybe the Lucifer Cannon. 
Now it is not an energy weapon as you may expect, instead it is a grenade 
launcher for siege the alien structures. 
It can carry four little grenades. That grenades have less power than the 
manual but enougth for destroy buildings.
Actually there are a bug with the Lucifer Cannon, it cannot use the primary
fire. I was intended to let the player charge and next shoot the grenade with
the power of the charge (if you charge more, the grenade will go more far 
away). But that does not worked, so for now you can only fire the Lucifer 
Cannon with the secondary button only (the bots know how to do that :P).
The hand grenade just have an integrated alien detector. It does not explode
with a timer as the original one, it only will explode when a alien get near.
That is useful for making a grenades field.


NEW ADMIN COMMANDS
------------------

There are some new admin commands:

\node 
You can add, delete, and connect nodes with this command.
There are documentation and some examples in the PBOT webpage/forum.
Try with google. If you really don't understand how it works I can write a detailed 
explanation in the forums, but for now:
\node add <- Add a node in the place where you are.
\node del <- Delete a node in the place where you are.
\node connect <- Prepares to connect the node in where you are. You must go to another
		 node to connect with.
So, really it is not too hard to undertand. The nodes you connect are the paths what
the bots will take, if you put nodes but don't connect anything then the bots don't
go to any place (they will stuck in the first node reached). When they have a path (a
node connected to another one) they will go to that node. If there are more than one
connection they will go to the most oldest one.
You must practice before understand how the bots can reach stairs and complex ways.
\node action <- Set the action in that node. You will give help if you write only
		\node action
\node save <- Don't forget this command if not your changes will be lost.

!node 
!node devdraw
It was intended to replace \node but actually only draw the nodes so you can see it in 
the map. That was a nice improvement because creating nodes in blind is a hard work.
Run !node devdraw to draw the actual nodes, run it again to clean the drawing.

!bot
There are some improvement in this command, for example you can delete all bots with: 
!bot del * <- Deletes all bots in the game. Usefull for clean the server instantly when 
	      you are putting nodes and don't want bots bothering you.

!nextmap
This command just finish the map as a draw game and continues in the next map found in 
maprotation.

!credits name amount
Give free credits to a player. Useful for testing purposes where you don't want to
wait to get credits. I used it mostly for test bots buying or for fun giving to me
being an alien evos to evolve to the most powerful one and kill humans :P


MORE INFO
---------
For more information please refer to the example files in the directory "examples".

