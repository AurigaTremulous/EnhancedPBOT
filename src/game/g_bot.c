/*
===========================================================================
Copyright (C) 2007 Amine Haddad

This file is part of Tremulous.

The original works of vcxzet (lamebot3) were used a guide to create TremBot.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/* Current version: v0.7 */

// Aliens natural movement (smoothed)
// Si no se define ejecuta el codigo viejo (movimientos bruscos al apuntar)
// Ejecuta el nuevo codigo para apuntar que simula la utilizacion
// de un mouse. Ese efecto da un movimiento mas natural cuando un
// alien tiene que apuntar. Al mismo tiempo ayuda a encontrar caminos
// ya que no cambia de rumbo bruscamente. Esto se puede resumir
// en que hace a los aliens un poco mas inteligentes.
// Si tienes un procesador potente te recomiendo activarlo de lo contrario
// puede que consuma un poco mas de CPU.
#define ALIENS_NATURAL 1

// TODO
// Al seguir a un amigo ir reconociendo los paths viejos?

#include "g_local.h"

#ifndef RAND_MAX
#define RAND_MAX 32768
#endif
void G_BotReload( gentity_t *ent, int clientNum )
{
	ClientDisconnect( clientNum );
	G_BotAdd( ent->client->pers.netname, ent->client->pers.teamSelection, 
	ent->botSkillLevel, clientNum );
	//trap_SendServerCommand( -1, "print \"Bot interfiriendo slot de jugador recargado\n\"" );
}

void G_DeleteBots( void )
{
	int i;
	gentity_t *bot;
	bot = &g_entities[ 0 ];
	for( i = 0; i < level.maxclients; i++, bot++ )
	{
		if(bot->r.svFlags & SVF_BOT) 
		{ClientDisconnect(bot->client->ps.clientNum);}
	}
}

void G_BotAdd( char *name, int team, int skill, int ignore ) {
	int i,i2;
	int clientNum;
	char userinfo[MAX_INFO_STRING], rate[MAX_STRING_CHARS];
	char guid[33]="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	int reservedSlots = 0;
	int aimspeed = 0, aimspeedn = 0, aimspeedpath = 0, aimspeedpathn = 0;
	float pbsuit = -1, ppsaw = -1, pshotgun = -1, plasgun = -1;
	float pmdriver = -1, pchaingun = -1, pprifle = -1, pflamer = -1, plcannon = -1;
	float pfriend = -1, pdefend = -1;
	gentity_t *bot;
	
	
	// Carga la personalidad del bot
	char filename[ MAX_QPATH ];
	char *token, *p;
	int len;
	fileHandle_t fileHandle;
	static char	buffer [ 2000 ];
	Com_sprintf( filename, sizeof( filename ), "bot/%s.cfg", name );
	
	len = trap_FS_FOpenFile( filename, &fileHandle, FS_READ );
	if (fileHandle)
	{
	    if ( len <= 2000 )
	    {
		trap_FS_Read( buffer, len, fileHandle );
		buffer[len] = 0;
		trap_FS_FCloseFile( fileHandle );
		p = buffer;
		
		while (1)
		{
		    token = COM_ParseExt( &p, qtrue );
		    // EOF
		    if ( !token[0] )
		    {
			break;
		    }
		    
		    if ( !strcmp( token, "botSkillLevel" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    skill = atoi(token);
		    }
		    else if ( !strcmp( token, "botTeam" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			{
			    if ( !Q_stricmp(token, "humans") )
			    {
				team = PTE_HUMANS;
			    }
			    else if ( !Q_stricmp(token, "aliens") )
			    {
				team = PTE_ALIENS;

			    }
			}
		    }
		    else if ( !strcmp( token, "botRate" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			{
			    Q_strncpyz( rate, token, sizeof(rate) );
			}
			else
			{    
			    trap_Cvar_VariableStringBuffer( "sv_MaxRate", rate, sizeof(rate) );
			}
		    }
		    else if ( !strcmp( token, "botGUID" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			{
			    Q_strncpyz(guid, token, sizeof(guid));
			}
		    }
		    else if ( !strcmp( token, "botAimSpeed" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    aimspeed = atoi(token);
		    }
		    else if ( !strcmp( token, "botAimSpeedNear" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    aimspeedn = atoi(token);
		    }
		    else if ( !strcmp( token, "botAimSpeedPath" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    aimspeedpath = atoi(token);
		    }
		    else if ( !strcmp( token, "botAimSpeedPathNear" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    aimspeedpathn = atoi(token);
		    }
		    else if ( !strcmp( token, "botPrefFriend" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pfriend = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefDefend" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pdefend = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefBattleSuit" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pbsuit = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefPainSaw" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    ppsaw = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefShotgun" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pshotgun = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefLasgun" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    plasgun = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefMassDriver" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pmdriver = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefChaingun" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pchaingun = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefPulseRifle" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pprifle = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefFlamer" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    pflamer = atof(token);
		    }
		    else if ( !strcmp( token, "botPrefLuciferCannon" ) )
		    {
			token = COM_ParseExt( &p, qfalse );
			if ( token[0] )
			    plcannon = atof(token);
		    }
		}
		G_Printf ( va( S_COLOR_YELLOW "Se lee personalidad de %s\n", name ) );
	    }
	    else // Len >
	    {
		G_Printf ( va( S_COLOR_YELLOW "NO se lee personalidad de %s porque el archivo es mayor a 2000 bytes\n", name ) );
		trap_FS_FCloseFile( fileHandle );
	    }
	}

	//Los puestos reservados son para los aliens, los humanos usan puestos publicos
	reservedSlots = trap_Cvar_VariableIntegerValue( "sv_privateClients" );
	if (team == PTE_HUMANS)
	{
	    // find what clientNum to use for humans bot
	    clientNum = -1;
	    for( i = (level.maxclients-1); i >= reservedSlots; i-- ) {
		if( i == ignore )
		{continue;}
		if( !g_entities[i].inuse ) {
			clientNum = i;
			break;
		}
	    }
	}
	else
	{
	    // find what clientNum to use for aliens bot
	    clientNum = -1;
	    for( i = (reservedSlots-1); i >= 0; i-- ) {
		if( i == ignore )
		{continue;}
		if( !g_entities[i].inuse ) {
			clientNum = i;
			break;
		}
	    }
	}
	
	if(clientNum == 0)
	{
		trap_Printf("Warning: Bots will be reloaded if they interfere with client connections\n");
	}
	if(clientNum < 0) {
		trap_Printf("no more slots for bot\n");
		return;
	}
	
	bot = &g_entities[ clientNum ];
	bot->inuse = qtrue;
	
	//default bot data
	bot->botCommand = BOT_REGULAR;
	bot->botFriend = NULL;
	bot->botEnemy = NULL;
	bot->botFriendLastSeen = 0;
	bot->botEnemyLastSeen = 0;
	
	if(skill > 10){skill = 10;} // Skill alto para bots sumamente tontos (sale 1 por defecto)
	if(skill < 0){skill = 0;}
	bot->botSkillLevel = skill;
	
	if (aimspeed <= 0) { aimspeed=1700+(int)random()*2000; }
	bot->botAimSpeed = aimspeed;
	if (aimspeedn <= 0) { aimspeedn=270+(int)random()*300; }
	bot->botAimSpeedNear = aimspeedn;
	if (aimspeedpath <= 0) { aimspeedpath=2800+(int)random()*2200; }
	bot->botAimSpeedPath = aimspeedpath;
	if (aimspeedpathn <= 0) { aimspeedpathn=280+(int)random()*300; }
	bot->botAimSpeedPathNear = aimspeedpathn;

	if (pfriend < 0) { pfriend = 0.10f; }
	bot->botPrefFriend = pfriend;
	
	if (pdefend < 0) { pdefend = 0.0f; }
	bot->botPrefDefend = pdefend;
	
	if (pbsuit < 0) { pbsuit=0.14f; }
	bot->botPrefBattleSuit = pbsuit;
	//if (phelmet < 0) { phelmet=; }
	//if (plarmour < 0) { plarmour=; }
	//if (pbattp < 0) { pbattp=; }
	//if (pmgun < 0) { pmgun=; }
	if (ppsaw < 0) { ppsaw=0.04f; }
	bot->botPrefPainSaw = ppsaw;
	
	if (pshotgun < 0) { pshotgun=0.50f; }
	bot->botPrefShotgun = pshotgun;
	
	if (plasgun < 0) { plasgun=0.60f; }
	bot->botPrefLasgun = plasgun;
	
	if (pmdriver < 0) { pmdriver=0.58f; }
	bot->botPrefMassDriver = pmdriver;
	
	if (pchaingun < 0) { pchaingun=0.53f; }
	bot->botPrefChaingun = pchaingun;
	
	if (pprifle < 0) { pprifle=0.55f; }
	bot->botPrefPulseRifle = pprifle;
	
	if (pflamer < 0) { pflamer=0.12f; }
	bot->botPrefFlamer = pflamer;
	
	if (plcannon < 0) { plcannon=0.05f; }
	bot->botPrefLuciferCannon = plcannon;
	
	bot->botTeam = team;
	bot->pathChosen = qfalse;
	bot->nextNode = qfalse;
	//bot->numOldPaths = 0;
	bot->state = FINDNEWPATH;
	bot->blocked = qfalse;
	bot->pain  = bot_pain; // Para detectar enemigo cuando le disparan
	//bot->touch = bot_touch; // Para detectar cuando lo toca un companiero, asi se corre

	for(i2 = 0;i2 < MAX_PATHS;i2++)
	{
		bot->SkipPaths[i2] = -1;
		bot->APaths[i2] = 0;
	}
	bot->i_APaths = 1; // Activamos el modo generacion de camino
	
	// register user information
	userinfo[0] = '\0';
	Info_SetValueForKey( userinfo, "name", name );
	//trap_Cvar_VariableStringBuffer( "sv_MaxRate", rate, sizeof(rate) );
	Info_SetValueForKey( userinfo, "rate", rate );
	Info_SetValueForKey( userinfo, "snaps", "10" );
	Info_SetValueForKey( userinfo, "cl_guid", guid );
	Info_SetValueForKey( userinfo, "ip", "127.0.0.1" );
	trap_SetUserinfo( clientNum, userinfo );
	
	// have it connect to the game as a normal client
	if(ClientConnect(clientNum, qtrue, qtrue) != NULL ) {
		// won't let us join
		return;
	}

	bot->r.svFlags |= SVF_BOT;

	ClientBegin( clientNum );
	G_ChangeTeam( bot, team );
	if (team == PTE_HUMANS)
	{
	    bot->client->ps.isHumanBot = qtrue;
	    G_LogOnlyPrintf("ClientTeamClass: %i human %s\n", clientNum, "rifle");
	}
	else
	{
	    bot->client->ps.isHumanBot = qfalse;
	}
}

void G_BotDel( int clientNum ) {
	gentity_t *bot;

	bot = &g_entities[clientNum];
	if( !( bot->r.svFlags & SVF_BOT ) ) {
		trap_Printf( va("'^7%s^7' is not a bot\n", bot->client->pers.netname) );
		return;
	}
	bot->inuse = qfalse;
	ClientDisconnect(clientNum);
}

void G_BotToggle( int clientNum ) {
	gentity_t *player;

	player = &g_entities[clientNum];
	if( !( player->r.svFlags & SVF_BOT ) ) {
	    player->r.svFlags |= SVF_BOT;
	    trap_Printf( va("'^7%s^7' is set as a bot\n", player->client->pers.netname) );
	    return;
	}
	else
	{
	    player->r.svFlags &= ~SVF_BOT;
	    trap_Printf( va("'^7%s^7' is set as a normal player\n", player->client->pers.netname) );
	    return;	    
	}
}

void G_BotCmd( gentity_t *master, int clientNum, char *command ) {
  gentity_t *bot;
  
  bot = &g_entities[clientNum];
  if( !( bot->r.svFlags & SVF_BOT ) ) {
    return;
  }
  
  bot->botFriend = NULL;
  bot->botEnemy = NULL;
  bot->botFriendLastSeen = 0;
  bot->botEnemyLastSeen = 0;
  
  //ROTAX
  if (/*g_ambush.integer == */0)
  {
    trap_Printf( va("You can't change aliens behavior in ambush mod\n") );
    return;
    }
  
  if( !Q_stricmp( command, "regular" ) ) {
	  
    bot->botCommand = BOT_REGULAR;
  	//trap_SendServerCommand(-1, "print \"regular mode\n\"");
  	
   } else if( !Q_stricmp( command, "idle" ) ) {
	   
    bot->botCommand = BOT_IDLE;
    //trap_SendServerCommand(-1, "print \"idle mode\n\"");
    
  } else if( !Q_stricmp( command, "attack" ) ) {
	  
    bot->botCommand = BOT_ATTACK;
    //trap_SendServerCommand(-1, "print \"attack mode\n\"");
    
  } else if( !Q_stricmp( command, "standground" ) ) {
	  
    bot->botCommand = BOT_STAND_GROUND;
    //trap_SendServerCommand(-1, "print \"stand ground mode\n\"");
    
  } else if( !Q_stricmp( command, "defensive" ) ) {
	  
    bot->botCommand = BOT_DEFENSIVE;
    //trap_SendServerCommand(-1, "print \"defensive mode\n\"");
    
  } else if( !Q_stricmp( command, "followprotect" ) ) {
	  
    bot->botCommand = BOT_FOLLOW_FRIEND_PROTECT;
    bot->botFriend = master;
    //trap_SendServerCommand(-1, "print \"follow-protect mode\n\"");
    
  } else if( !Q_stricmp( command, "followattack" ) ) {
	  
    bot->botCommand = BOT_FOLLOW_FRIEND_ATTACK;
    bot->botFriend = master;
    //trap_SendServerCommand(-1, "print \"follow-attack mode\n\"");
    
  } else if( !Q_stricmp( command, "followidle" ) ) {
	  
    bot->botCommand = BOT_FOLLOW_FRIEND_IDLE;
    bot->botFriend = master;
    //trap_SendServerCommand(-1, "print \"follow-idle mode\n\"");
    
  } else if( !Q_stricmp( command, "teamkill" ) ) {
	  
    bot->botCommand = BOT_TEAM_KILLER;
    //trap_SendServerCommand(-1, "print \"team kill mode\n\"");
    
  } else {
	
    bot->botCommand = BOT_REGULAR;
    //trap_SendServerCommand(-1, "print \"regular (unknown) mode\n\"");
    
  }
  
  return;
}

int distanceToTargetNode( gentity_t *self )
{
	int distance,Ax,Ay,Az,Bx,By,Bz = 0;
	Ax = level.paths[self->targetPath].coord[0];
	Ay = level.paths[self->targetPath].coord[1];
	Az = level.paths[self->targetPath].coord[2];
	Bx = self->s.pos.trBase[0];
	By = self->s.pos.trBase[1];
	Bz = self->s.pos.trBase[2];
	distance = sqrt((Ax - Bx)*(Ax - Bx) + (Ay - By)*(Ay - By) + (Az - Bz)*(Az - Bz));
	return distance;
}

qboolean botAimAtTarget( gentity_t *self, gentity_t *target ) {
	int Ax,Ay,Az,Bx,By,Bz = 0;
	vec3_t dirToTarget, angleToTarget;
	vec3_t top = { 0, 0, 0};
	vec3_t  forward, right, up;
	vec3_t  muzzle;
	int i, aimspeed, sinresult;
	
	AngleVectors( self->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( self, forward, right, up, muzzle );
	if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL3 || 
		self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL3_UPG)
	{
		Ax = target->s.pos.trBase[0];
		Ay = target->s.pos.trBase[1];
		Az = target->s.pos.trBase[2];
		Bx = self->s.pos.trBase[0];
		By = self->s.pos.trBase[1];
		Bz = self->s.pos.trBase[2];
		if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL3)
		{top[2] = (int)(sqrt((Ax - Bx)*(Ax - Bx) + (Ay - By)*(Ay - By) + (Az - Bz)*(Az - Bz)) / 3);}
		else
		{top[2] = (int)(sqrt((Ax - Bx)*(Ax - Bx) + (Ay - By)*(Ay - By) + (Az - Bz)*(Az - Bz)) / 5);}
	}
	VectorAdd( target->s.pos.trBase, top, top);
	VectorSubtract( top, muzzle, dirToTarget );
	VectorNormalize( dirToTarget );
	vectoangles( dirToTarget, angleToTarget );

		    /*DEBUG
		    char namebuff[32];
		    Q_strncpyz(namebuff, self->client->pers.netname, sizeof(namebuff));
		    Q_CleanStr(namebuff);
		    if (!Q_stricmp(namebuff, "Soldado"))
		    {
			trap_SendServerCommand(-1, va("print \"viewangles: %f; e: %f; angle: %f\n\"", 
			self->client->ps.viewangles[1],
			SHORT2ANGLE(self->client->delta_angles_e[1]),
			self->s.angles[1]
			)
			 );
		    }*/


	// Hey! no me gustan los bots roboticos asi que voy a simular un mouse
	// como si el bot humano fuera alguien jugando.
#ifndef ALIENS_NATURAL
	if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
	{
#endif
	    aimspeed = self->botAimSpeed; // 900
#ifdef ALIENS_NATURAL
	    //if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS) { aimspeed=2000; }
#endif
 	    self->client->delta_angles_e[ 0 ] = ANGLE2SHORT( angleToTarget[ 0 ] );
	    self->client->delta_angles_e[ 1 ] = ANGLE2SHORT( angleToTarget[ 1 ] );
	    self->client->ps.delta_angles[ 2 ] = ANGLE2SHORT( angleToTarget[ 2 ] );
	    for (i = 0; i <= 1; i++)
	    {
		sinresult = (int)(sin(SHORT2ANGLE(self->client->delta_angles_e[i] - self->client->ps.delta_angles[i])/180*M_PI)*1000);
		if ( sinresult > 0 )
		{
		    if (sinresult >= 250) // Rapido
		    {
			self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] + aimspeed;
		    }
		    else // Mas lento
		    {
			if ( sinresult >= 5 )
			{
			    if (VectorLength( self->client->ps.velocity ) < 30.0f &&
				level.time - self->client->bottimeTask > (2000 + 500 * self->botSkillLevel))
			    {
				self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] + 100;
				if (level.time - self->client->bottimeTask > 6000)
				    self->client->bottimeTask = level.time+(int)random()*3000;
			    }
			    else
			    if (sinresult >= 22)
			    {
				self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] + self->botAimSpeedNear;// 260;
			    }
			}
		    }
		}
		else
		{
		    if (sinresult <= -250) // Rapido
		    {
			self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] - aimspeed;
		    }
		    else // Mas lento
		    {
			if ( sinresult <= -5 )
			{
			    if (VectorLength( self->client->ps.velocity ) < 30.0f &&
				level.time - self->client->bottimeTask > (2000 + 500 * self->botSkillLevel))
			    {
				self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] - 100;
				if (level.time - self->client->bottimeTask > 6000)
				    self->client->bottimeTask = level.time+(int)random()*3000;
			    }
			    else
			    if (sinresult <= -22)
			    {
				self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] - self->botAimSpeedNear;// 260;
			    }
			}
		    }
		}
	    }
#ifndef ALIENS_NATURAL
	}
	else
	{
 	    self->client->ps.delta_angles[ 0 ] = ANGLE2SHORT( angleToTarget[ 0 ] );
	    self->client->ps.delta_angles[ 1 ] = ANGLE2SHORT( angleToTarget[ 1 ] );
	    self->client->ps.delta_angles[ 2 ] = ANGLE2SHORT( angleToTarget[ 2 ] );
	    //VectorCopy( self->client->ps.viewangles, angleToTarget );
	}
#endif
	return qtrue;
}

qboolean botAimAtPath( gentity_t *self )
{
	vec3_t dirToTarget, angleToTarget;
	vec3_t top = { 0, 0, 0};
	int vh = 0;
	int i, aimspeed, sinresult;
	BG_FindViewheightForClass(  self->client->ps.stats[ STAT_PCLASS ], &vh, NULL );
	top[2]=vh;
	VectorAdd( self->s.pos.trBase, top, top );
	VectorSubtract( level.paths[self->targetPath].coord, top, dirToTarget );
	VectorNormalize( dirToTarget );
	vectoangles( dirToTarget, angleToTarget );

#ifndef ALIENS_NATURAL    
    if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
    {
#endif
 	self->client->delta_angles_e[ 0 ] = ANGLE2SHORT( angleToTarget[ 0 ] );
	self->client->delta_angles_e[ 1 ] = ANGLE2SHORT( angleToTarget[ 1 ] );
	self->client->ps.delta_angles[ 2 ] = ANGLE2SHORT( angleToTarget[ 2 ] );
	aimspeed=self->botAimSpeedPath; // 4000
#ifdef ALIENS_NATURAL
	//if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS) { aimspeed=5000; }
#endif
	for (i = 0; i <= 1; i++)
	{
	    if (i == 0 && self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS &&
		distanceToTargetNode(self) < 200)
		{ continue; }
	    sinresult = (int)(sin(SHORT2ANGLE(self->client->delta_angles_e[i] - self->client->ps.delta_angles[i])/180*M_PI)*1000);
	    if ( sinresult > 0 )
	    {
	        if (sinresult >= 300) // Rapido
		{
		    self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] + aimspeed;
		}
		else // Mas lento
		{
		    if (sinresult >= 26)
		    {
			self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] + self->botAimSpeedPathNear; //300
		    }
		}
	    }
	    if ( sinresult < 0 )
	    {
	        if (sinresult <= -300) // Rapido
		{
		    self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] - aimspeed;
		}
		else // Mas lento
		{
		    if (sinresult <= -26)
		    {
			self->client->ps.delta_angles[i] = self->client->ps.delta_angles[i] - self->botAimSpeedPathNear;
		    }
		}
	    }
	}
#ifndef ALIENS_NATURAL
    }
    else
    {
 	self->client->ps.delta_angles[ 0 ] = ANGLE2SHORT( angleToTarget[ 0 ] );
	self->client->ps.delta_angles[ 1 ] = ANGLE2SHORT( angleToTarget[ 1 ] );
	self->client->ps.delta_angles[ 2 ] = ANGLE2SHORT( angleToTarget[ 2 ] );
	//VectorCopy( self->client->ps.viewangles, angleToTarget );
	//VectorCopy( self->s.angles, angleToTarget );
    }
#endif

    return qtrue;
}

void G_FrameAim( gentity_t *self )
{
	if( !( self->r.svFlags & SVF_BOT ) )
	{
		return;
	}
	if (self->botEnemy)
	{
	    if(self->botEnemy->health <= 0)
	    {
		self->botEnemy = NULL;
		if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
		{
		    self->botCommand = BOT_IDLE_ATTACKING;
		    self->client->bottimeTask = level.time-500*self->botSkillLevel;
		    self->client->bottimeFriend = level.time;
		}
	    }
	    else
	    {
		if(botTargetInRange( self, self->botEnemy ) == qfalse)
		{
		    self->botEnemy = NULL;
    		    if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
		    {
			self->botCommand = BOT_IDLE_ATTACKING;
			self->client->bottimeTask = level.time-500*self->botSkillLevel;
			self->client->bottimeFriend = level.time;
		    }
		}
	    }
	}
	if (self->botFriend)
	{
	    if(self->botFriend->health <= 0)
		self->botFriend = NULL;
	    else
	    {
		if( botFriendInRange( self, self->botFriend ) == qfalse )
		{
		    if (level.time - self->client->bottimeFriend > (500+500*self->botSkillLevel)+3000)
		    {
			self->botFriend = NULL;
			//self->client->bottimeFriend = level.time+(int)random()*2000;
		    }
		}
		else
		{
		    if (level.time - self->client->bottimeFriend > (500+500*self->botSkillLevel)+1000)
		    {
			self->client->bottimeFriend = level.time-(500+500*self->botSkillLevel);
		    }
		}
	    }
	}
	if(!self->botEnemy && !self->botFriend && self->state == TARGETPATH &&
	    (level.time - self->client->bottimeTask > 500+500*self->botSkillLevel))
	{
	    /*//DEBUG
	    char namebuff[32];
	    Q_strncpyz(namebuff, self->client->pers.netname, sizeof(namebuff));
	    Q_CleanStr(namebuff);
	    if (!Q_stricmp(namebuff, "SoldadoUniversal"))
	    {
		trap_SendServerCommand(-1, va("print \"botAimAtPath self->targetPath: %d\n\"", self->targetPath) );
	    }*/
	    botAimAtPath(self);
	}
	else if(self->botEnemy)
	{
	    botAimAtTarget(self, self->botEnemy);
	}
	else if(self->botFriend && (level.time - self->client->bottimeFriend > 500+500*self->botSkillLevel))
	{
	    botAimAtTarget(self, self->botFriend);
	    //self->client->bottimeFriend = level.time-(500+500*self->botSkillLevel);
	}
}

// Verifica si es seguro avanzar o retroceder y devuelve el valor
// si es asi.
// Por si no te diste cuenta, esta funcion se encarga de:
// * Verificar que no haya un objeto delante que se vaya a chocar
// * Verificar que no haya un precipicio delante que le haga caer (*1)
// * Igual que las anteriores pero hacia atras (*1)
// (*1) = TODO (PARA HACER)
int botForwardIfSecure( gentity_t *self, int forwardMove )
{
    //vec3_t	dirToTarget, angleToTarget;
    //vec3_t	top = { 0, 0, 0};
    vec3_t	forward, right, up;
    vec3_t	muzzle, end;
    vec3_t	mins, maxs;
    trace_t	tr;
    
    // Solucion temprana: Si esta agachado no verifica nada
    // Esto ayuda a que los bots puedan pasar dentro de tuneles
    if ( self->client->ps.pm_flags & PMF_DUCKED )
	return forwardMove;

    if ( forwardMove > 0 )
    {
	VectorSet( mins, -4, -4, -4 );
	VectorSet( maxs, 4, 4, 4 );
	AngleVectors( self->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( self, forward, right, up, muzzle );
	VectorMA( muzzle, 30, forward, end );
	trap_Trace( &tr, muzzle, mins, maxs, end, self->s.number, MASK_SHOT );
	//traceEnt = &g_entities[ tr.entityNum ];
	if (tr.fraction >= 1.0)
	{
	    return forwardMove;
	}	
    }
    else if ( forwardMove < 0 )
    {
	/*VectorSet( mins, -10, -10, -10 );
	VectorSet( maxs, 10, 10, 10 );
	AngleVectors( self->client->ps.delta_angles, forward, right, up );
	CalcMuzzlePoint( self, forward, right, up, muzzle );
	VectorMA( muzzle, 20, forward, end );
	dot = DotProduct( , tr.plane.normal )
	trap_Trace( &tr, muzzle, mins, maxs, end, self->s.number, MASK_SHOT );*/
	//traceEnt = &g_entities[ tr.entityNum ];
	//if (tr.fraction >= 1.0)
	{
	    return forwardMove;
	}	
    }
    
    return 0;
}

void G_FastThink( gentity_t *self )
{
	int forwardMove = 127;
	if( !( self->r.svFlags & SVF_BOT ) )
	{
		return;
	}
	
	if(self->botEnemy || self->botFriend)
	{
		self->enemytime = level.time;
	}
	if(!self->botEnemy && !self->botFriend && self->state == TARGETPATH)
	{
		self->client->pers.cmd.buttons = 0;
		//self->client->pers.cmd.upmove = 0;
		self->client->pers.cmd.rightmove = 0;
		if (distanceToTargetNode(self) > 50)
		{
		    self->client->pers.cmd.forwardmove = botForwardIfSecure( self, forwardMove );
		}
		else
		{
		    self->client->pers.cmd.forwardmove = 0;
		}
		if((self->isblocked == qtrue || 
			VectorLength( self->client->ps.velocity ) < 50.0f) && level.time - self->enemytime > 1000)
		{
			//srand( trap_Milliseconds( ) );
			if (random() < 0.05f)
			{	
			    self->client->pers.cmd.buttons |= BUTTON_GESTURE;
			}
			//self->client->pers.cmd.forwardmove = 0;
		        if(self->client->time10000 >= 9000)
    			    self->client->pers.cmd.rightmove = -100;
  			if(self->client->time10000 >= 4000 && self->client->time10000 <= 5000)
  			    self->client->pers.cmd.rightmove = 100;
			
			// Bloqueado
			if( level.time - self->jumptime > 900 &&
				(( self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS && self->client->ps.stats[ STAT_STAMINA ] > 500 
				 && (random() < 0.64f)) ||
				self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS))
			{
				self->client->pers.cmd.forwardmove = botForwardIfSecure( self, forwardMove );
				self->client->pers.cmd.upmove = 20;
				if(level.time - self->jumptime > 1800)
				{
				    self->jumptime = level.time;
				    self->client->pers.cmd.upmove = 0;
				}
			}
		}
		if(self->lastpathid >= 0)
		{
			switch(level.paths[self->lastpathid].action)
			{
				case BOT_JUMP:	if(level.time - self->jumptime > 600)
						{
							self->client->pers.cmd.upmove = 20;
							if(level.time - self->jumptime > 1000)
							{
							    self->client->pers.cmd.upmove = 0;
							    self->jumptime = level.time;
							    self->lastpathid = 0;
							}
						}
				break;
				case BOT_WALLCLIMB: if( BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) ) {
							self->client->pers.cmd.upmove = -1;
							}
						break;
				case BOT_KNEEL: if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
						{
							self->client->pers.cmd.upmove = -1;
						}
						break;
				case BOT_POUNCE:if(self->client->pers.classSelection == PCL_ALIEN_LEVEL3 && 
							self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_SPEED)
							self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
						else if(self->client->pers.classSelection == PCL_ALIEN_LEVEL3_UPG && 
							self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_UPG_SPEED)
							self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
						break;
				default: break;
			}
		}
		if(distanceToTargetNode(self) < 70)
		{
			self->state = FINDNEXTPATH;
		}
		if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS && 
			self->client->ps.stats[ STAT_STAMINA ] < MAX_STAMINA/3)
		{
		    self->client->pers.cmd.upmove = -1;
		    self->client->pers.cmd.buttons = 0;
		    self->client->pers.cmd.rightmove = 0;
		    self->client->pers.cmd.forwardmove = 0;
		    self->client->bottimeTask = level.time+(int)random()*500;
		    self->botCommand = BOT_STAND_GROUND;
		}
	}
}

void findnewpath( gentity_t *self )
{
	trace_t trace;
	int i,i2,distance,Ax,Ay,Az,Bx,By,Bz = 0;
	int closestpath = 0;
	int closestpathdistance = 2000;
	qboolean pathfound = qfalse, delpath = qfalse;
	
	self->timeFoundPath = level.time;
	for(i = 0; i < level.numPaths; i++) //find a nearby path that wasn't used before
	{
		if(self->SkipPaths[i] > 0){continue;}
		for (i2 = 0; i2 < 5; i2++)
		{
		    if (level.paths[i].nextid[i2] < 0) {delpath=qtrue; break;}
		}
		if (delpath == qtrue)
		{
		    delpath = qfalse;
		    continue;
		}
		
		trap_Trace( &trace, self->s.pos.trBase, NULL, NULL, level.paths[i].coord, self->s.number, MASK_PLAYERSOLID );
		//trap_Trace( &trace, muzzle, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
		if( trace.fraction < 1.0 )
		{continue;}
		
		Ax = level.paths[i].coord[0];
		Ay = level.paths[i].coord[1];
		Az = level.paths[i].coord[2];
		Bx = self->s.pos.trBase[0];
		By = self->s.pos.trBase[1];
		Bz = self->s.pos.trBase[2];
		
		distance = sqrt((Ax - Bx)*(Ax - Bx) + (Ay - By)*(Ay - By) + (Az - Bz)*(Az - Bz));
		if(distance < 3000)
		{
			if(distance < closestpathdistance)
			{
				closestpath = i;
				closestpathdistance = distance;
				pathfound = qtrue;
			}
		}
	}
	if(pathfound == qtrue)
	{
		self->targetPath = closestpath;
		self->OldPaths[self->targetPath].time = level.time;
		self->state = TARGETPATH;
		self->isblocked = qfalse;
	}
	else
	{
		self->state = LOST;
		self->client->pers.cmd.forwardmove = 0;
		if (self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS)
		{
		    self->client->pers.cmd.upmove = -1;
		}
		else
		{
		    self->client->pers.cmd.upmove = 0;
		}
		self->client->pers.cmd.rightmove = 0;
		self->client->pers.cmd.buttons = 0;
		if (random() < 0.03f)
		{	
		    self->client->pers.cmd.buttons |= BUTTON_GESTURE;
		    for(i = 0;i < MAX_PATHS;i++)
			{
				self->SkipPaths[i] = -1;
		    }
		}
	}
	return;
}

void findnextpath( gentity_t *self )
{
    int i;
    int nextpath = 0;
    int timeold;
    self->lastpathid = self->targetPath;
    timeold = level.time;
    for(i = 0; i < MAX_PATH_NODES; i++)
    {
	    if(level.paths[self->targetPath].nextid[i] < level.numPaths &&
	    level.paths[self->targetPath].nextid[i] >= 0)
	    {
		if ((self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS) && 
		    (level.humans_timeapaths > level.time))
		{
		    if (level.humans_apaths[level.paths[self->targetPath].nextid[i]] >= (level.humans_apaths[self->targetPath]+1))
		    {
			nextpath = level.paths[self->targetPath].nextid[i];
			break;
		    }
		}
		else if ( (self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0) &&
			  (self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0_UPG) &&
			  (self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_LEVEL0) &&
			  (level.aliens_timeapaths > level.time) )
		{
		    if (level.aliens_apaths[level.paths[self->targetPath].nextid[i]] >= (level.aliens_apaths[self->targetPath]+1))
		    {
			nextpath = level.paths[self->targetPath].nextid[i];
			break;
		    }
		}
		
	    	if( (self->OldPaths[level.paths[self->targetPath].nextid[i]].time <= timeold) )
	    	{
	    	    timeold = self->OldPaths[level.paths[self->targetPath].nextid[i]].time;
	    	    nextpath = level.paths[self->targetPath].nextid[i];
	    	}		
	    }
    }
    self->targetPath = nextpath;
    self->OldPaths[self->targetPath].time = level.time;
    self->state = TARGETPATH;
    self->timeFoundPath = level.time;

    // Guardamos el camino
    if (self->i_APaths > 0)
    {
	self->APaths[nextpath] = self->i_APaths;
	self->i_APaths++;
    }

	//DEBUG
        /*char namebuff[32];
	Q_strncpyz(namebuff, self->client->pers.netname, sizeof(namebuff));
	Q_CleanStr(namebuff);
	if (!Q_stricmp(namebuff, "Soldado"))
	{
	    trap_SendServerCommand(-1, va("print \"nextpath: %d, time: %d\n\"", 
	    self->targetPath,
	    self->OldPaths[self->targetPath].time));
	}*/
}

void checktimepath( gentity_t *self )
{
    int timeout;
    double mod = 1.0f;

    // Sin llegar a destino
    if (self->lastpathid >= 0)
    {
	timeout = level.paths[self->lastpathid].timeout;
    }
    else
    {
	timeout = 4000;
    }

    // Con el battlesuit es mas lento, tiene que esperar mas tiempo
    if (BG_InventoryContainsUpgrade( UP_BATTLESUIT, self->client->ps.stats ))
    {
	mod = BSUIT_SPEEDMOD;
    }

    if ((level.time - self->timeFoundPath)*mod > timeout)
    {
	//self->client->bottimePath = 0;
	self->timeFoundPath = level.time;
	self->state = FINDNEWPATH;
	self->client->pers.cmd.forwardmove = 0;
	self->client->pers.cmd.upmove = 0;
	self->client->pers.cmd.rightmove = 0;
	self->client->pers.cmd.buttons = 0;
	if (random() < 0.01f)
	{	
	    self->client->pers.cmd.buttons |= BUTTON_GESTURE;
	}
	
	if(self->targetPath > -1)
    	    self->SkipPaths[self->targetPath] = 1;	
    }
    return;
}

void pathfinding( gentity_t *self )
{
	switch(self->state)
	{
		case FINDNEWPATH: findnewpath(self); break;
		case FINDNEXTPATH: findnextpath(self); break;
		case LOST: findnewpath(self);break; //LOL :(
		case TARGETPATH: 
		default: checktimepath(self); break;
	}
}
/*
  ========
  bot_pain
  ========
*/
void bot_pain ( gentity_t *self, gentity_t *attacker, int damage )
{
    if( self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS)
    {
	// Los aliens detectan la amenaza cuando le pegan
	if( !self->botEnemy && attacker->client &&
    	attacker->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
	{
	    self->botEnemy = attacker;
	}
	
	// Y pasan esa informacion a sus escoltas
	if( self->botFriend && !self->botFriend->botEnemy &&
	attacker->client &&
    	attacker->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
	{
	    self->botFriend->botEnemy = attacker;
	}
    }
    else
    {
    }
}

/*
  =========
  bot_touch
  =========
  ACTUALMENTE NO SE USA, es mejor calcular una distancia prudencial
  minima a la que el bot se te acercara para que no quede totalmente pegado
*/
void bot_touch( gentity_t *ent, gentity_t *other, trace_t *trace)
{
    if (other->client)
    {
	// Se corre hacia atras
	ent->client->pers.cmd.forwardmove = botForwardIfSecure( ent, -100 );
    }
}

// Borra todo el camino marcado del bot y establece que 
// se debe marcar un nuevo camino si i_apaths > 0
void botDelPersonalTravel( gentity_t *self, int i_apaths )
{
    int n;
    self->i_APaths = i_apaths;
    if (i_apaths > 0)
    {
		for (n = 0; n < MAX_PATHS; n++)
		{
			self->APaths[n] = 0;
		}
    }
}

qboolean Bot_BuyEnergyAmmo( gentity_t *self )
{
    #define USE_OBJECT_RANGE 64

      int       entityList[ MAX_GENTITIES ];
      vec3_t    range = { USE_OBJECT_RANGE, USE_OBJECT_RANGE, USE_OBJECT_RANGE };
      vec3_t    mins, maxs;
      int       i, num;
      gentity_t *traceEnt;
      int 	weapon = WP_BLASTER;
    
      /*for ( i = WP_NONE; i < WP_NUM_WEAPONS; i++ )
      {
        if ( BG_InventoryContainsWeapon(i, self->client->ps.stats )
	{
	    if (BG_FindUsesEnegyForWeapon( i ))
	    {
		weapon = i;
		break;
	    }
	}
      }*/
      
      if ( BG_InventoryContainsWeapon( WP_LUCIFER_CANNON, self->client->ps.stats ) )
      {
        weapon = WP_LUCIFER_CANNON;
      }
      else if ( BG_InventoryContainsWeapon( WP_PULSE_RIFLE, self->client->ps.stats ) )
      {
        weapon = WP_PULSE_RIFLE;
      }
      else if ( BG_InventoryContainsWeapon( WP_MASS_DRIVER, self->client->ps.stats ) )
      {
        weapon = WP_MASS_DRIVER;
      }
      else if ( BG_InventoryContainsWeapon( WP_LAS_GUN, self->client->ps.stats ) )
      {
        weapon = WP_LAS_GUN;
      }
      
    
      if (weapon == WP_BLASTER)
    	return qfalse;
	
        //no entity in front of player - do a small area search
        VectorAdd( self->client->ps.origin, range, maxs );
        VectorSubtract( self->client->ps.origin, range, mins );

        num = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
        for( i = 0; i < num; i++ )
        {
          traceEnt = &g_entities[ entityList[ i ] ];

          if( traceEnt && traceEnt->biteam == self->client->ps.stats[ STAT_PTEAM ] && traceEnt->use )
          {
            traceEnt->use( traceEnt, self, self ); //other and activator are the same in this context
	    self->buytime = level.time+(int)random()*4000;
	    
	    G_ForceWeaponChange( self, weapon );
            return qtrue;
          }
        }    
    return qfalse;
}

qboolean Bot_Buy( gentity_t *self )
{
	qboolean boughtweap = qfalse;
	qboolean boughtup = qfalse;
	qboolean buybatt = qfalse;
	int weapon,upgrade;
	int maxAmmo, maxClips;
	int clientNum = self->client - level.clients;
	if(self->client->ps.stats[ STAT_PTEAM ] != PTE_HUMANS){return qfalse;}
	upgrade = UP_LIGHTARMOUR;
	if(BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ))
	{
		BG_RemoveUpgradeFromInventory( upgrade, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForUpgrade( upgrade ), qfalse );
	}
	upgrade = UP_HELMET;
	if(BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ))
	{
		BG_RemoveUpgradeFromInventory( upgrade, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForUpgrade( upgrade ), qfalse );
	}
	upgrade = UP_BATTPACK;
	if(BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ))
	{
		BG_RemoveUpgradeFromInventory( upgrade, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForUpgrade( upgrade ), qfalse );
	}
	weapon = WP_MACHINEGUN;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		//G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_CHAINGUN;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_LAS_GUN;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_LUCIFER_CANNON;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_FLAMER;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_PULSE_RIFLE;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_MASS_DRIVER;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_SHOTGUN;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	weapon = WP_PAIN_SAW;
	if( BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) )
	{
		BG_RemoveWeaponFromInventory( weapon, self->client->ps.stats );
		G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( weapon ), qfalse );
	}
	
	if(g_humanStage.integer == 2 && random() < /*0.15f*/ self->botPrefBattleSuit)
	{
		upgrade = UP_BATTLESUIT;
		if(!BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ) && 
			// Al menos tiene que tener dinero extra para una chaingun tambien
			(BG_FindPriceForUpgrade( upgrade ) + BG_FindPriceForUpgrade( WP_CHAINGUN )) <= (short)self->client->ps.persistant[ PERS_CREDIT ] && 
			!(BG_FindSlotsForUpgrade( upgrade ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			BG_AddUpgradeToInventory( upgrade, self->client->ps.stats );
			G_AddCreditToClient( self->client, -(short)BG_FindPriceForUpgrade( upgrade ), qfalse );
		}
	}
	upgrade = UP_LIGHTARMOUR;
	if(!BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ) && 
		BG_FindPriceForUpgrade( upgrade ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
		!(BG_FindSlotsForUpgrade( upgrade ) & self->client->ps.stats[ STAT_SLOTS ]) && 
		!BG_InventoryContainsUpgrade( UP_BATTLESUIT, self->client->ps.stats ))
	{
		BG_AddUpgradeToInventory( upgrade, self->client->ps.stats );
		G_AddCreditToClient( self->client, -(short)BG_FindPriceForUpgrade( upgrade ), qfalse );
	}
	if(g_humanStage.integer >= 1)
	{
		upgrade = UP_HELMET;
		if(!BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ) && 
			BG_FindPriceForUpgrade( upgrade ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForUpgrade( upgrade ) & self->client->ps.stats[ STAT_SLOTS ]) && 
			!BG_InventoryContainsUpgrade( UP_BATTLESUIT, self->client->ps.stats ))
		{
			BG_AddUpgradeToInventory( upgrade, self->client->ps.stats );
			G_AddCreditToClient( self->client, -(short)BG_FindPriceForUpgrade( upgrade ), qfalse );
		}
	}
	if(g_humanStage.integer == 2 && g_bot_lcannon.integer > 0 && random() < /*0.05f*/ self->botPrefLuciferCannon)
	{
		weapon = WP_LUCIFER_CANNON;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
			buybatt = qtrue;
		}
	}
	if(g_humanStage.integer == 1 && boughtweap == qfalse && g_bot_flamer.integer > 0 && random() < /*0.12f*/ self->botPrefFlamer)
	{
		weapon = WP_FLAMER;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
		}
	}
	if(g_humanStage.integer == 1 && boughtweap == qfalse && g_bot_prifle.integer > 0 && random() < /*0.55f*/ self->botPrefPulseRifle)
	{
		weapon = WP_PULSE_RIFLE;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
			buybatt = qtrue;
		}
	}
	if(boughtweap == qfalse && g_bot_chaingun.integer > 0 && random() < /*0.55f*/ self->botPrefChaingun)
	{
		weapon = WP_CHAINGUN;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
		}
	}
	if(boughtweap == qfalse && g_bot_mdriver.integer > 0 && random() < /*0.58f*/ self->botPrefMassDriver)
	{
		weapon = WP_MASS_DRIVER;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
			buybatt = qtrue;
		}
	}
	if(boughtweap == qfalse && g_bot_lasgun.integer > 0 && random() < /*0.60f*/ self->botPrefLasgun)
	{
		weapon = WP_LAS_GUN;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
			buybatt = qtrue;
		}
	}
	if(boughtweap == qfalse && g_bot_shotgun.integer > 0 && random() < /*0.50f*/ self->botPrefShotgun)
	{
		weapon = WP_SHOTGUN;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
		}
	}
	if(boughtweap == qfalse && g_bot_psaw.integer > 0 && random() < /*0.10f*/ self->botPrefPainSaw)
	{
		weapon = WP_PAIN_SAW;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
		}
	}
	if(boughtweap == qfalse && g_bot_mgun.integer > 0)
	{
		weapon = WP_MACHINEGUN;
		if( !BG_InventoryContainsWeapon( weapon, self->client->ps.stats ) &&
			BG_FindPriceForWeapon( weapon ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
			!(BG_FindSlotsForWeapon( weapon ) & self->client->ps.stats[ STAT_SLOTS ]))
		{
			boughtweap = qtrue;
		}
	}
	if(boughtweap == qtrue)
	{
		BG_AddWeaponToInventory( weapon, self->client->ps.stats );
		BG_FindAmmoForWeapon( weapon, &maxAmmo, &maxClips );
		if( BG_FindUsesEnergyForWeapon( weapon ) )
		{
		    if (BG_InventoryContainsUpgrade( UP_BATTPACK, self->client->ps.stats ) )
			maxAmmo = (int)( (float)maxAmmo * BATTPACK_MODIFIER );
		
		    if (BG_InventoryContainsUpgrade( UP_BATTLESUIT, self->client->ps.stats ) )
			maxAmmo = (int)( (float)maxAmmo * BSUIT_BATTPACK_MODIFIER );
		}
		else
		{
		    // In Invasion the Battlesuit have an ammo chain addition for more ammo in weapons
    		    if (BG_InventoryContainsUpgrade( UP_BATTLESUIT, self->client->ps.stats ))
    		    {
        		if (BG_InventoryContainsWeapon( WP_MACHINEGUN, self->client->ps.stats ) )
            		maxAmmo += BSUIT_AMMOCHAIN_RIFLE;
        		else
        		if (BG_InventoryContainsWeapon( WP_CHAINGUN, self->client->ps.stats ) )
            		maxAmmo += BSUIT_AMMOCHAIN;
    		    }
		}

		BG_PackAmmoArray( weapon, self->client->ps.ammo, self->client->ps.powerups,
                      maxAmmo, maxClips );
		G_AddCreditToClient( self->client, -(short)BG_FindPriceForWeapon( weapon ), qfalse );
		G_ForceWeaponChange( self, weapon );
	}
	else
	{
		weapon = WP_BLASTER;
		G_ForceWeaponChange( self, weapon );
	}
	upgrade = UP_BATTPACK;
	if(!BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ) && 
		BG_FindPriceForUpgrade( upgrade ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
		!(BG_FindSlotsForUpgrade( upgrade ) & self->client->ps.stats[ STAT_SLOTS ]) && 
		!BG_InventoryContainsUpgrade( UP_BATTLESUIT, self->client->ps.stats ) && 
		buybatt == qtrue)
	{
		BG_AddUpgradeToInventory( upgrade, self->client->ps.stats );
		G_AddCreditToClient( self->client, -(short)BG_FindPriceForUpgrade( upgrade ), qfalse );
	}
	if(g_humanStage.integer >= 1 && g_bot_grenade.integer > 0)
	{
	    upgrade = UP_GRENADE;
	    if(!BG_InventoryContainsUpgrade( upgrade, self->client->ps.stats ) && 
		BG_FindPriceForUpgrade( upgrade ) <= (short)self->client->ps.persistant[ PERS_CREDIT ] &&
		!(BG_FindSlotsForUpgrade( upgrade ) & self->client->ps.stats[ STAT_SLOTS ]))
	    {
		BG_AddUpgradeToInventory( upgrade, self->client->ps.stats );
		G_AddCreditToClient( self->client, -(short)BG_FindPriceForUpgrade( upgrade ), qfalse );
	    }
	}

	G_GiveClientMaxAmmo( self, qtrue );
	if(boughtup == qtrue || boughtweap == qtrue)
	{ClientUserinfoChanged( clientNum );}
	self->buytime = level.time;
	return qtrue;
}

extern qboolean G_RoomForClassChange( gentity_t *ent, pClass_t class, vec3_t newOrigin );
void Bot_Evolve( gentity_t *self )
{
	vec3_t origin;
	qboolean classfound = qfalse;
	int class;
	int levels;
	int clientNum;
	if(self->client->ps.stats[ STAT_PTEAM ] != PTE_ALIENS){return;}
	clientNum = self->client - level.clients;
	class = PCL_ALIEN_LEVEL4;
	levels = BG_ClassCanEvolveFromTo( self->client->ps.stats[ STAT_PCLASS ],
                                           class,
                                           (short)self->client->ps.persistant[ PERS_CREDIT ], 0 );
	if(BG_ClassIsAllowed( class ) && 
		BG_FindStagesForClass( class, g_alienStage.integer ) && 
		level.overmindPresent &&
		G_RoomForClassChange( self, class, origin ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) &&
		levels >= 0)
	{
		classfound = qtrue;
	}
	if(classfound == qfalse)
	{
	class = PCL_ALIEN_LEVEL3_UPG;
	levels = BG_ClassCanEvolveFromTo( self->client->ps.stats[ STAT_PCLASS ],
                                           class,
                                           (short)self->client->ps.persistant[ PERS_CREDIT ], 0 );
	if(BG_ClassIsAllowed( class ) && 
		BG_FindStagesForClass( class, g_alienStage.integer ) && 
		level.overmindPresent &&
		G_RoomForClassChange( self, class, origin ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) &&
		levels >= 0)
	{
		classfound = qtrue;
	}
	}
	if(classfound == qfalse)
	{
	class = PCL_ALIEN_LEVEL3;
	levels = BG_ClassCanEvolveFromTo( self->client->ps.stats[ STAT_PCLASS ],
                                           class,
                                           (short)self->client->ps.persistant[ PERS_CREDIT ], 0 );
	if(BG_ClassIsAllowed( class ) && 
		BG_FindStagesForClass( class, g_alienStage.integer ) && 
		level.overmindPresent &&
		G_RoomForClassChange( self, class, origin ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) &&
		levels >= 0)
	{
		classfound = qtrue;
	}
	}
	if(classfound == qfalse)
	{
	class = PCL_ALIEN_LEVEL2_UPG;
	levels = BG_ClassCanEvolveFromTo( self->client->ps.stats[ STAT_PCLASS ],
                                           class,
                                           (short)self->client->ps.persistant[ PERS_CREDIT ], 0 );
	if(BG_ClassIsAllowed( class ) && 
		BG_FindStagesForClass( class, g_alienStage.integer ) && 
		level.overmindPresent &&
		G_RoomForClassChange( self, class, origin ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) &&
		levels >= 0)
	    {
		classfound = qtrue;
	    }
	}
	if(classfound == qfalse)
	{
	class = PCL_ALIEN_LEVEL2;
	levels = BG_ClassCanEvolveFromTo( self->client->ps.stats[ STAT_PCLASS ],
                                           class,
                                           (short)self->client->ps.persistant[ PERS_CREDIT ], 0 );
	if(BG_ClassIsAllowed( class ) && 
		BG_FindStagesForClass( class, g_alienStage.integer ) && 
		level.overmindPresent &&
		G_RoomForClassChange( self, class, origin ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) &&
		levels >= 0)
	{
		classfound = qtrue;
	}
	}
	if(classfound == qfalse)
	{
	class = PCL_ALIEN_LEVEL1_UPG;
	levels = BG_ClassCanEvolveFromTo( self->client->ps.stats[ STAT_PCLASS ],
                                           class,
                                           (short)self->client->ps.persistant[ PERS_CREDIT ], 0 );
	if(BG_ClassIsAllowed( class ) && 
		BG_FindStagesForClass( class, g_alienStage.integer ) && 
		level.overmindPresent &&
		G_RoomForClassChange( self, class, origin ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) &&
		levels >= 0)
	{
		classfound = qtrue;
	}
	}
	if(classfound == qfalse)
	{
	class = PCL_ALIEN_LEVEL1;
	levels = BG_ClassCanEvolveFromTo( self->client->ps.stats[ STAT_PCLASS ],
                                           class,
                                           (short)self->client->ps.persistant[ PERS_CREDIT ], 0 );
	if(BG_ClassIsAllowed( class ) && 
		BG_FindStagesForClass( class, g_alienStage.integer ) && 
		level.overmindPresent &&
		G_RoomForClassChange( self, class, origin ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBING ) && 
		!( self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) &&
		levels >= 0)
	{
		classfound = qtrue;
	}
	}
	if(classfound == qtrue)
	{
		self->client->pers.evolveHealthFraction = (float)self->client->ps.stats[ STAT_HEALTH ] /
		(float)BG_FindHealthForClass( self->client->ps.stats[ STAT_PCLASS ] );
		if( self->client->pers.evolveHealthFraction < 0.0f )
			self->client->pers.evolveHealthFraction = 0.0f;
		else if( self->client->pers.evolveHealthFraction > 1.0f )
			self->client->pers.evolveHealthFraction = 1.0f;
		G_AddCreditToClient( self->client, -(short)levels, qtrue );
		self->client->pers.classSelection = class;
		ClientUserinfoChanged( clientNum );
		VectorCopy( origin, self->s.pos.trBase );
		ClientSpawn( self, self, self->s.pos.trBase, self->s.apos.trBase );	
		self->evolvetime = level.time;
		return;
	}
	self->evolvetime = level.time;
	return;
}

int botFindClosestFriend( gentity_t *self );
void G_BotThink( gentity_t *self )
{
	int distance = 0;
	int tDistance = 0; // Para disparar no de tan cerca
	int maxdist = 350;
	int uhyb = 0;
	int clicksToStopChase = 40; //30 = 5 seconds
	//int tooCloseDistance = 100; // about 1/3 of turret range
	int tooCloseDistanceFriend = 90;
	int forwardMove = 127; // max speed
	int tempEntityIndex = -1;
	//trace_t   tr;
	//vec3_t    end;
	//vec3_t    mins, maxs;
	//vec3_t  forward, right, up;
	//vec3_t  muzzle;
	//gentity_t *traceEnt;
	qboolean followFriend = qfalse;	
	
	if( !( self->r.svFlags & SVF_BOT ) )
	{
		return;
	}
	
	//self->client->pers.cmd.buttons = 0;
	//self->client->pers.cmd.forwardmove = 0;
	//self->client->pers.cmd.upmove = 0;
	//self->client->pers.cmd.rightmove = 0;
	
	if (self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS && 
	    !self->botFriend &&
	    level.time - self->client->bottimeTask > 34000)
	{
		//self->client->bottimeTask = level.time+(int)random()*5000;
		/*if ( random() < 0.06f )
		{
		    self->botCommand = BOT_STAND_GROUND;
		}*/
		/*else
		if ( random() < 0.5f )
		{
		    self->botCommand = BOT_DEFENSIVE;
		}*/
		/*else
		if ( random() < 0.3f )
		{
		    self->botCommand = BOT_IDLE;
		}*/
	}
	
	self->isblocked = qfalse;
	/*if(self->state == TARGETPATH && !self->botEnemy && !self->botFriend)
	{
		//AngleVectors( self->client->ps.delta_angles, forward, right, up );
		//CalcMuzzlePoint( self, forward, right, up, muzzle );
		
		VectorSet( mins, -20, -20, -20 );
		VectorSet( maxs, 20, 20, 20 );
		AngleVectors( self->client->ps.delta_angles, forward, right, up );
		CalcMuzzlePoint( self, forward, right, up, muzzle );
		VectorMA( muzzle, 20, forward, end );
		trap_Trace( &tr, muzzle, mins, maxs, end, self->s.number, MASK_SHOT );
		traceEnt = &g_entities[ tr.entityNum ];
		if(traceEnt->health > 0)
		{self->isblocked = qtrue;}
		else
		{self->isblocked = qfalse;}
	}*/
	if(self->botEnemy)
	{
	    //self->client->pers.cmd.buttons = 0;
	    self->client->pers.cmd.forwardmove = 0;
	    self->client->pers.cmd.upmove = 0;
	    self->client->pers.cmd.rightmove = 0;
	    //self->timeFoundPath = level.time;
	    // reset botEnemy if enemy is dead
	    if(self->botEnemy->health <= 0) {
		self->botEnemy = NULL;
		if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
		{
		    self->botCommand = BOT_IDLE_ATTACKING;
		    self->client->bottimeTask = level.time-500*self->botSkillLevel;
		    self->client->bottimeFriend = level.time;
		}
	    }
	    else
	    {
		if(botTargetInRange( self, self->botEnemy ) == qfalse)
		{
			self->botEnemy = NULL;
			if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
			{
			    self->botCommand = BOT_IDLE_ATTACKING;
			    self->client->bottimeTask = level.time-500*self->botSkillLevel;
			    self->client->bottimeFriend = level.time;
			}
		}
	    }
	}
	if(self->botFriend)
	{
		self->client->pers.cmd.buttons = 0;
		self->client->pers.cmd.forwardmove = 0;
		self->client->pers.cmd.rightmove = 0;
		if (self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
		{
		    if (self->botFriend->client->pers.cmd.upmove <= 0)
			self->client->pers.cmd.upmove = self->botFriend->client->pers.cmd.upmove;
		    if ( self->botFriend->client->ps.stats[ STAT_STATE ] & SS_SPEEDBOOST )
		    {
			self->client->ps.stats[ STAT_STATE ] |= SS_SPEEDBOOST;
		    }
		    else
		    {
			self->client->ps.stats[ STAT_STATE ] &= ~SS_SPEEDBOOST;
		    }
		}
		else
		    self->client->pers.cmd.upmove = self->botFriend->client->pers.cmd.upmove;

		//self->timeFoundPath = level.time;
	}	
	// if friend dies or in alien change class, reset status to regular
	if( self->botFriend && (self->botFriend->health <= 0 ||
	(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS && 
	self->botFriend->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0 &&
	self->botFriend->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0_UPG)) ) {
		self->botFriend = NULL;
	}
	
	if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
	{
	    // De alguna manera el servidor me borra ps
	    // cuando el bot se spawnea.
	    self->client->ps.isHumanBot = qtrue; 
	    
	    //Use blaster if weapon out of ammo
	    if( self->noammo == qtrue &&
		self->client->ps.weapon != WP_BLASTER)
	    {G_ForceWeaponChange( self, WP_BLASTER );}
	    //Use other weapon if now have ammo
	    if( self->noammo == qfalse &&
		self->client->ps.weapon == WP_BLASTER )
	    {
		int weapon;
		for (weapon = WP_MACHINEGUN; weapon <=WP_LUCIFER_CANNON; weapon++)
		{
		    G_ForceWeaponChange( self, weapon);
		}
	    }
	    
	    //Use medkit if low hp
	    if(BG_InventoryContainsUpgrade( UP_MEDKIT, self->client->ps.stats ) )
	    if (self->health <= 40 || (self->health<60 && (random() <= 0.07f)))
	    {
		BG_ActivateUpgrade( UP_MEDKIT, self->client->ps.stats );
	    }
	
	    // Shoot the grenade!
	    //If alien buildable near then shoot the grenade (for that the bot
	    // buy one!)
	    //If low in HP then shoot the grenade anyway if not we will lost it 
	    // and money for nothing (and chicks for free?)
	    if(BG_InventoryContainsUpgrade( UP_GRENADE, self->client->ps.stats ) &&
	    ( G_BuildableRange(self->client->ps.origin, 150, BA_A_SPAWN) || 
	     G_BuildableRange(self->client->ps.origin, 150, BA_A_ACIDTUBE) ||
	    ((self->health <= 30) && (random() <= 0.60f))) && 
		!G_BuildableRange(self->client->ps.origin, 200, BA_H_SPAWN) && 
		!G_BuildableRange(self->client->ps.origin, 200, BA_H_REACTOR) &&
		!G_BuildableRange(self->client->ps.origin, 200, BA_H_ARMOURY) &&
		!G_BuildableRange(self->client->ps.origin, 200, BA_H_MEDISTAT) &&
		!G_BuildableRange(self->client->ps.origin, 200, BA_H_REPEATER) &&
		!G_BuildableRange(self->client->ps.origin, 200, BA_H_MGTURRET)
	    )
	    {
		int lastWeapon = self->s.weapon;
		//remove grenade
		BG_DeactivateUpgrade( UP_GRENADE, self->client->ps.stats );
		BG_RemoveUpgradeFromInventory( UP_GRENADE, self->client->ps.stats );

		//M-M-M-M-MONSTER HACK
		self->s.weapon = WP_GRENADE;
		FireWeapon( self );
		self->s.weapon = lastWeapon;
	    }
	
	    //Buy stuff from arm if within range
	    if(G_BuildableRange( self->client->ps.origin, 100, BA_H_ARMOURY ) && 
		level.time - self->buytime > 20000) // Cada 20 segundos asi junta un poco de plata
	    {
		Bot_Buy(self);
		self->client->bottimeTask = level.time + 250 + (int)random()*2000;
		self->botCommand = BOT_IDLE;
		// Solucion temprana:
		//self->state = -1;
	        self->client->pers.cmd.buttons = 0;
	        self->client->pers.cmd.forwardmove = 0;
	        self->client->pers.cmd.upmove = 0;
	        self->client->pers.cmd.rightmove = 0;	
	    }

	    // Compra ammo si esta cerca de un repetidor o el reactor
	    if( ( G_BuildableRange(self->client->ps.origin, 64, BA_H_REPEATER) || 
	      G_BuildableRange(self->client->ps.origin, 64, BA_H_REACTOR) ) && 
		self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS &&
		level.time - self->buytime > 10000)
	    {
	        if (Bot_BuyEnergyAmmo(self))
	        {
		    self->client->bottimeTask = level.time + (int)random()*5000;
		    self->botCommand = BOT_IDLE;
		    // Solucion temprana:
		    //self->state = -1;
	    	    self->client->pers.cmd.buttons = 0;
	    	    self->client->pers.cmd.forwardmove = 0;
	    	    self->client->pers.cmd.upmove = 0;
	    	    self->client->pers.cmd.rightmove = 0;
		}
	    }
	
	    if (G_BuildableRange( self->client->ps.origin, 15, BA_H_MEDISTAT ) &&
	    self->health < 100)
	    {
		self->client->bottimeTask = level.time + 250;
		self->botCommand = BOT_IDLE;
		// Solucion temprana:
		//self->state = -1;
	    	self->client->pers.cmd.buttons = 0;
	    	self->client->pers.cmd.forwardmove = 0;
	    	self->client->pers.cmd.upmove = 0;
		self->client->pers.cmd.rightmove = 0;
	    }
	}
	
	if(g_bot_evolve.integer > 0 &&
	    level.time - self->evolvetime > 10000 &&
	    self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS &&
	    self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0 &&
	    self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0_UPG
		)
	{Bot_Evolve(self);}
	if(self->client->ps.weapon == WP_MACHINEGUN && g_bot_mgun.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_SHOTGUN && g_bot_shotgun.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_PAIN_SAW && g_bot_psaw.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_LAS_GUN && g_bot_lasgun.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_MASS_DRIVER && g_bot_mdriver.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_PULSE_RIFLE && g_bot_prifle.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_FLAMER && g_bot_flamer.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_LUCIFER_CANNON && g_bot_lcannon.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	if(self->client->ps.weapon == WP_CHAINGUN && g_bot_chaingun.integer <= 0)
	{
		G_ForceWeaponChange( self, WP_BLASTER );
	}
	// what mode are we in?
	switch(self->botCommand) {
			
		case BOT_FOLLOW_FRIEND_PROTECT:
			// run towards friend, attack enemy
			//break;
			
		case BOT_FOLLOW_FRIEND_ATTACK:
			// run with friend until enemy spotted, then rush enemy
			// Intenta mantenerse con el amigo
			if ( self->botEnemy && self->botFriend && botGetDistanceBetweenPlayer(self, self->botFriend) > 300 )
			{
			    self->botEnemy = NULL;
			}

			if(self->botEnemy) {
				// we already have an enemy. See if still in LOS.
				if(!botTargetInRange(self,self->botEnemy)) {
					// if it's been over clicksToStopChase clicks since we last seen him in LOS then do nothing, else follow him!
					if(self->botEnemyLastSeen > clicksToStopChase) {
						// forget him!
						self->botEnemy = NULL;
						self->botEnemyLastSeen = 0;
					} else {
						//chase him
						self->botEnemyLastSeen++;
					}
				} else {
					// we see him!
					self->botEnemyLastSeen = 0;
				}
				
				//if we are chasing enemy, reset counter for friend LOS .. if its true
				if(self->botEnemy) {
					if(botFriendInRange(self,self->botFriend)) {
						self->botFriendLastSeen = 0;
					} else {
						self->botFriendLastSeen++;
					}
				}
			}
			if (!self->botEnemy || (level.time - self->searchtime > 2200 + 375 * self->botSkillLevel))
			{
			    if ( !self->botFriend || (self->botFriend && botGetDistanceBetweenPlayer(self, self->botFriend) <= 250) )
			    {
				self->client->pers.cmd.buttons = 0;
				if(level.time - self->searchtime > 375 * self->botSkillLevel)
				{
				    self->searchtime = level.time;
				    // try to find closest enemy
				    tempEntityIndex = botFindClosestEnemy(self, qfalse);
			
				    if(tempEntityIndex >= 0)
					self->botEnemy = &g_entities[tempEntityIndex];
				}
			    }
			}
			if (!self->botEnemy)
			{
				// no enemy
				if(self->botFriend) {
					// Verifica si su amigo tiene enemigo
					// y lo convierte en su enemigo tambien.
					// Esto solo debe funcionar con los aliens
					// y quiza de vez en cuando con los humanos
					if (self->botFriend->botEnemy && self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS)
					{
					    self->botEnemy = self->botFriend->botEnemy;
					}
					
					// see if our friend is in LOS
					if(botFriendInRange(self,self->botFriend)) {
						// go to him!
						followFriend = qtrue;
						self->botFriendLastSeen = 0;
					} else {
						// if it's been over clicksToStopChase clicks since we last seen him in LOS then do nothing, else follow him!
						if(self->botFriendLastSeen > clicksToStopChase) {
							// forget him!
							followFriend = qfalse;
						} else {
							self->botFriendLastSeen++;
							followFriend = qtrue;
						}
					}

					if(followFriend) {	
						//botAimAtTarget(self, self->botFriend);
						distance = botGetDistanceBetweenPlayer(self, self->botFriend);
						// enable wallwalk
						if( BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) ) {
							self->client->pers.cmd.upmove = -1;
						}
						
						//botShootIfTargetInRange(self,self->botEnemy);
						if(distance>tooCloseDistanceFriend) {
							self->client->pers.cmd.forwardmove = botForwardIfSecure( self, forwardMove );
							/*self->client->pers.cmd.rightmove = -100;
							if(self->client->time1000 >= 500)
								self->client->pers.cmd.rightmove = 100;*/
						    if(VectorLength( self->client->ps.velocity ) < 50.0f)
						    {
							if (random() < 0.03f)
							{	
							    self->client->pers.cmd.buttons |= BUTTON_GESTURE;
							}
		    					if(self->client->time10000 >= 9000)
    							    self->client->pers.cmd.rightmove = -100;
  							if(self->client->time10000 >= 4000 && self->client->time10000 <= 5000)
  							    self->client->pers.cmd.rightmove = 100;
							// FOLLOW_FRIEND_ATTACK blocked
							if( level.time - self->jumptime > 900 && 
							  (( self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS && self->client->ps.stats[ STAT_STAMINA ] > 500
							     && (random() < 0.80f)) ||
							    self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS))
							{
							    self->client->pers.cmd.upmove = 20;
							    if(level.time - self->jumptime > 1800)
							    {
								self->client->pers.cmd.upmove = 0;
								self->jumptime = level.time;
							    }
							}
						    }
						    
						    if (BG_InventoryContainsUpgrade( UP_BATTLESUIT, self->client->ps.stats ))
							maxdist = 450;
						    
						    if ( distance>maxdist )
						    {
							self->botFriend = NULL;
							followFriend = qfalse;
						    }
						} else {
							// Se mueve hacia los costados del amigo
							// Esto es util ya que si el bot esta a espaldas de su amigo
							// no podra atacar enemigos que siempre vienen por el frente
							// en cambio al estar a los costados va a poder hacerlo
  							if((self->client->time10000 >= 9000) || 
							    (self->client->time10000 >= 4000 && self->client->time10000 <= 5000) )
							{
							    self->client->pers.cmd.rightmove = 100;
							}
  							if((self->client->time10000 <= 1000) ||
							    (self->client->time10000 >= 6000 && self->client->time10000 <= 7000) )
							{
							    self->client->pers.cmd.rightmove = -100;
							}
							// Si esta demasiado cerca se aleja
							// para mantener una distancia prudencial
							if(distance<(tooCloseDistanceFriend/2)) {
							self->client->pers.cmd.forwardmove = botForwardIfSecure( self, -100 );
							}
							
						}
					}
				}
				else
				{
				    self->botCommand = BOT_REGULAR;
				}
			}
			if (self->botEnemy) {
			// enemy!
				int tDistance; // Para disparar no de tan cerca
				
				botAimAtTarget(self, self->botEnemy);
				
				botShootIfTargetInRange(self,self->botEnemy);
				
				if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
				{
				    //distance = botGetDistanceBetweenPlayer(self, self->botEnemy);
					if (self->client->ps.weapon == WP_PAIN_SAW)
					{
					    tDistance = 70;
					}
					else if (self->client->ps.weapon == WP_FLAMER)
					{
					    tDistance = 180;
					}
					else if (self->client->ps.weapon == WP_SHOTGUN)
					{
					    tDistance = 150;
					}
					else if (self->client->ps.weapon == WP_CHAINGUN)
					{
					    tDistance = 270;
					}
					else
					{
					    tDistance = 400;
					}
					    
					if ( Distance( self->s.pos.trBase, self->botEnemy->s.pos.trBase ) < tDistance )
					{
					    // Se hace para atras
					    self->client->pers.cmd.forwardmove = botForwardIfSecure( self, -100 );
					    // Se mueve hacia los costados
					    if( g_human_strafe.integer > 0)
					    {
  						if(self->client->time10000 <= 2000)
						    self->client->pers.cmd.rightmove = -100;
  						if(self->client->time10000 >= 4000 && self->client->time10000 <= 6000)
  						    self->client->pers.cmd.rightmove = 100;
					    }
					}
					else
					{
					    // Se detiene
					    self->client->pers.cmd.forwardmove = 0;
					}
				}

				//ROTAX
				if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS)
				{
				    // enable wallwalk
				    if( BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) )
				    {
					self->client->pers.cmd.upmove = -1;
				    }
				    // FOLLOW_FRIEND_ATTACK enemy
				    else if( level.time - self->jumptime > 900 && random() < 0.75f &&
				    // Solucion temprana: Los builders no tienen tanto interes de ir a la lucha
					self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0 &&
					self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0_UPG
					)
				    {
					//self->client->ps.velocity[2] = BG_FindJumpMagnitudeForClass( self->client->ps.stats[ STAT_PCLASS ] );
					self->client->pers.cmd.upmove = 20;
					if(level.time - self->jumptime > 2000)
					{
					    self->client->pers.cmd.upmove = 0;
					    self->jumptime = level.time;
					}
				    }

				    //self->client->pers.cmd.upmove = -1;
				    self->client->pers.cmd.forwardmove = botForwardIfSecure( self, forwardMove );
				    if (g_ambush_dodge_random.integer <= 0)
				    	g_ambush_dodge_random.integer = 1;
  				    if (g_ambush_dodge_random.integer != 1 || g_ambush_dodge.integer > 0)
				    {
				        //srand( trap_Milliseconds( ) );
					uhyb = (g_ambush_dodge.integer + (rand() % g_ambush_dodge_random.integer));
					self->client->pers.cmd.rightmove = -uhyb;
    					if(self->client->time1000 >= 500)
    					    self->client->pers.cmd.rightmove = uhyb;
  				    }
				}
			}
			break;
			
		case BOT_FOLLOW_FRIEND_IDLE:
			// run with friend and stick with him no matter what. no attack mode.
			if(self->botFriend) {
				// see if our friend is in LOS
				if(botFriendInRange(self,self->botFriend)) {
					// go to him!
					followFriend = qtrue;
					self->botFriendLastSeen = 0;
				} else {
					// if it's been over clicksToStopChase clicks since we last seen him in LOS then do nothing, else follow him!
					if(self->botFriendLastSeen > clicksToStopChase) {
						// forget him!
						followFriend = qfalse;
					} else {
						//chase him
						self->botFriendLastSeen++;
						followFriend = qtrue;
					}
					
				}
				
				if(followFriend) {
					distance = botGetDistanceBetweenPlayer(self, self->botFriend);
					//botAimAtTarget(self, self->botFriend);
					
					// enable wallwalk
					if( BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) ) {
						self->client->pers.cmd.upmove = -1;
					}
					
					//botShootIfTargetInRange(self,self->botFriend);
					if(distance>tooCloseDistanceFriend) {
						self->client->pers.cmd.forwardmove = botForwardIfSecure( self, forwardMove );
						/*self->client->pers.cmd.rightmove = -100;
						if(self->client->time1000 >= 500)
							self->client->pers.cmd.rightmove = 100;*/
					}
				}
			}
			
			break;
			
		case BOT_TEAM_KILLER:
			// attack enemies, then teammates!
			if(self->botEnemy) {
				// we already have an enemy. See if still in LOS.
				if(!botTargetInRange(self,self->botEnemy)) {
					// if it's been over clicksToStopChase clicks since we last seen him in LOS then do nothing, else follow him!
					if(self->botEnemyLastSeen > clicksToStopChase) {
						// forget him!
						self->botEnemy = NULL;
						self->botEnemyLastSeen = 0;
					} else {
						//chase him
						self->botEnemyLastSeen++;
					}
				} else {
					// we see him!
					self->botEnemyLastSeen = 0;
				}
			}
			
			if(!self->botEnemy) {
				// try to find closest enemy
				tempEntityIndex = botFindClosestEnemy(self, qtrue);
				if(tempEntityIndex >= 0)
					self->botEnemy = &g_entities[tempEntityIndex];
			}
			
			if(!self->botEnemy) {
				// no enemy, we're all alone :(
			} else {
				// enemy!
				//distance = botGetDistanceBetweenPlayer(self, self->botEnemy);
				//botAimAtTarget(self, self->botEnemy);
				
				// enable wallwalk
				if( BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) ) {
					self->client->pers.cmd.upmove = -1;
				}
				
				botShootIfTargetInRange(self,self->botEnemy);
				self->client->pers.cmd.forwardmove = botForwardIfSecure( self, forwardMove );
				self->client->pers.cmd.rightmove = -100;
				if(self->client->time1000 >= 500)
					self->client->pers.cmd.rightmove = 100;
			}
			
			break;

		case BOT_IDLE:
		    // just stand there and look pretty.
		    self->client->pers.cmd.buttons = 0;
		    self->client->pers.cmd.forwardmove = 0;
		    self->client->pers.cmd.upmove = 0;
		    self->client->pers.cmd.rightmove = 0;
		case BOT_IDLE_ATTACKING:
		    if (level.time - self->client->bottimeTask > 250+500*self->botSkillLevel)
		    {
		        if (self->botFriend)
		        {
			    self->botCommand = BOT_FOLLOW_FRIEND_ATTACK;
			}
			else
			{
			    self->botCommand = BOT_REGULAR;
			}
		    }		    
		break;	
		case BOT_DEFENSIVE:
			// if there is an enemy around, rush them but not too far from where you are standing when given this command
			//break;
		case BOT_STAND_GROUND:
		    // stand ground but attack enemies if you can reach.
		    self->client->pers.cmd.upmove = -1;
		    if (level.time - self->client->bottimeTask > 0)
		    {
			self->client->pers.cmd.upmove = 0;
		        if (self->botFriend)
		        {
			    self->botCommand = BOT_FOLLOW_FRIEND_ATTACK;
			}
			else
			{
			    self->botCommand = BOT_REGULAR;
			}
		    }
		    break;
		case BOT_ATTACK:
			// .. not sure ..
			//break;
		case BOT_REGULAR:
		default:
			// if there is enemy around, rush them and attack.
			if(self->botEnemy) {
				// we already have an enemy. See if still in LOS.
				if(!botTargetInRange(self,self->botEnemy)) {
					// if it's been over clicksToStopChase clicks since we last seen him in LOS then do nothing, else follow him!
					if(self->botEnemyLastSeen > clicksToStopChase) {
						// forget him!
						self->botEnemy = NULL;
						self->botEnemyLastSeen = 0;
					} else {
						//chase him
						self->botEnemyLastSeen++;
					}
				} else {
					// we see him!
					self->botEnemyLastSeen = 0;
				}
			}

			
			if(!self->botEnemy || (level.time - self->searchtime > 3000 + 375 * self->botSkillLevel)) {
				self->client->pers.cmd.buttons = 0;
				// try to find closest enemy and friend
				if(level.time - self->searchtime > 375 * self->botSkillLevel)
				{
					self->searchtime = level.time;
					if (!self->botEnemy) {
					    tempEntityIndex = botFindClosestEnemy(self, qfalse);
					    if(tempEntityIndex >= 0)
						self->botEnemy = &g_entities[tempEntityIndex];
					}
					if (!self->botFriend && 
					    self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0 &&
		    			    self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0_UPG &&
					    self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_LEVEL0 &&
					    random() < self->botPrefFriend ) {
					    tempEntityIndex = botFindClosestFriend(self);
					    if(tempEntityIndex >= 0)
					    {
						self->botFriend = &g_entities[tempEntityIndex];
						self->botCommand = BOT_FOLLOW_FRIEND_ATTACK;
						self->client->bottimeFriend = level.time;
						botDelPersonalTravel(self, 0);
					    }
					}
				}
			}
			
			if(!self->botEnemy) {
				int modif = (int)(1000*(1-level.alienBuildPoints/g_alienBuildPoints.integer));
				if (self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS && 
				( self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0 ||
				  self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0_UPG )
				&& level.time - self->client->bottimeBuild > 6*modif )
				{
				    buildable_t buildable;
				    if ( !G_IsOvermindBuilt() )
				    {
					buildable = BG_FindBuildNumForName("overmind");
				    }
				    else
				    {
					if (random() < 0.60f)
					    buildable = BG_FindBuildNumForName("eggpod");
					else
					if (random() < 0.55f)
					    buildable = BG_FindBuildNumForName("acid_tube");
					else
					if (random() < 0.50f)
					    buildable = BG_FindBuildNumForName("hive");
					else
					if (random() < 0.50f)
					    buildable = BG_FindBuildNumForName("barricade");
					else
					if (random() < 0.60f)
					    buildable = BG_FindBuildNumForName("trapper");
					else
					if (random() < 0.80f)
					    buildable = BG_FindBuildNumForName("booster");
					else
					    buildable = BG_FindBuildNumForName("hovel");

				    }
				    self->client->bottimeBuild = level.time+(int)(random()*10*modif);
				    G_BuildIfValid(self, buildable);
				}
				pathfinding(self); //Roam the map!!!
			} else {
				if (self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
				{
				    self->timeFoundPath = level.time - (int)(random()*1200);
				    //self->state = FINDNEXTPATH;
				}
				else
				{
				    self->state = FINDNEWPATH;
				}
				// enemy!				
				//distance = botGetDistanceBetweenPlayer(self, self->botEnemy);

				botAimAtTarget(self, self->botEnemy);
							
				botShootIfTargetInRange(self, self->botEnemy);

				//ROTAX
				if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
				{
					if (self->client->ps.weapon == WP_PAIN_SAW)
					{
					    tDistance = 70;
					}
					else if (self->client->ps.weapon == WP_FLAMER)
					{
					    tDistance = 180;
					}
					else if (self->client->ps.weapon == WP_SHOTGUN)
					{
					    tDistance = 150;
					}
					else if (self->client->ps.weapon == WP_CHAINGUN)
					{
					    tDistance = 270;
					}
					else
					{
					    tDistance = 400;
					}
					    
					if(Distance( self->s.pos.trBase, self->botEnemy->s.pos.trBase ) < tDistance)
					{
					    // Se hace para atras
					    self->client->pers.cmd.forwardmove = botForwardIfSecure( self, -100 );
					    // Se mueve hacia los costados
					    if( g_human_strafe.integer > 0)
					    {
  						if(self->client->time10000 <= 2000)
						    self->client->pers.cmd.rightmove = -100;
  						if(self->client->time10000 >= 4000 && self->client->time10000 <= 6000)
  						    self->client->pers.cmd.rightmove = 100;
					    }
					}
					else
					{
					    // Se detiene
					    self->client->pers.cmd.forwardmove = 0;
					}
				}

				//ROTAX
				if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS)
				{
				    // enable wallwalk
				    if( BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) )
				    {
					self->client->pers.cmd.upmove = -1;
				    }
				    // REGULAR enemy
				    else if( level.time - self->jumptime > 900 && random() < 0.70f &&
				    // Solucion temprana: Los builders no tienen tanto interes de ir a la lucha
					self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0 &&
					self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_BUILDER0_UPG
					)
				    {
					//self->client->ps.velocity[2] = BG_FindJumpMagnitudeForClass( self->client->ps.stats[ STAT_PCLASS ] );
					self->client->pers.cmd.upmove = 20;
					if(level.time - self->jumptime > 2000)
					{
					    self->client->pers.cmd.upmove = 0;
					    self->jumptime = level.time;
					}
				    }

				    //self->client->pers.cmd.upmove = -1;
				    uhyb = 0;
				    self->client->pers.cmd.forwardmove = botForwardIfSecure( self, forwardMove );
				    if (g_ambush_dodge_random.integer <= 0)
				    	g_ambush_dodge_random.integer = 1;
  				    if (g_ambush_dodge_random.integer != 1 || g_ambush_dodge.integer > 0)
				    {
				        //srand( trap_Milliseconds( ) );
					uhyb = (g_ambush_dodge.integer + (rand() % g_ambush_dodge_random.integer));
					self->client->pers.cmd.rightmove = -uhyb;
    					if(self->client->time1000 >= 500)
    					    self->client->pers.cmd.rightmove = uhyb;
  				    }
				}
			}
		break;
			
	}
}

int Bot_GetAmbushAlienClass( void )
{
    switch (g_ambush_stage.integer)
    {
	case 0: //granger
	    return PCL_ALIEN_BUILDER0;
	case 1: //adv granger
	    return PCL_ALIEN_BUILDER0_UPG;
	case 2: //dretch
	    return PCL_ALIEN_LEVEL0;
	case 3: //basilisk
	    return PCL_ALIEN_LEVEL1;
	case 4://adv basilisk
	    return PCL_ALIEN_LEVEL1_UPG;
	case 5://marauder
	    return PCL_ALIEN_LEVEL2;
	case 6://adv marauder
	    return PCL_ALIEN_LEVEL2_UPG;
	case 7://dragon
	    return PCL_ALIEN_LEVEL3;
	case 8://adv dragon
	    return PCL_ALIEN_LEVEL3_UPG;
	case 9://tyrant
	    return PCL_ALIEN_LEVEL4;
    }
    
    return PCL_ALIEN_LEVEL0;
}

extern void ambush_next_stage( void );
void G_BotSpectatorThink( gentity_t *self ) {
	gentity_t *bot;
	int i = 0;
	int NumHumansPlayers = 0; 
	int NumHumansBots = 0;
	int numclients = 0;

  //self->client->ps.pm_type = PM_FREEZE;
  // Hay una mierda de que mato companyeros a lo loco
  // aunque ya estan muertos!!! como hago???
  // Una falla de mierda que me tiene harto que nunca
  // pude descubrir y seguramente la anyadio este
  // zapallo que hizo los bots (como otra falla de
  // memoria que tenia muy grave)
  // Con esto voy a intentar una solucion temprana.
  // A ver si considera que en modo espectador el
  // bot esta y sigue y seguira muerto!! 
  self->takedamage = qfalse;
  self->health = 0;
  
  if( self->client->ps.pm_flags & PMF_QUEUED ) 
  {
	  //we're queued to spawn, all good
	  return;
  }
    
  self->client->bottimeRespawn++;
  
  // Espera un momentito antes de elegir
  if ( self->client->bottimeRespawn < 20 )
	return;

        for( i = 0; i < level.maxclients; i++ )
        {
	    bot = &g_entities[ i ];
	    if (bot->inuse == qtrue)
	    {
		if ( bot->r.svFlags & SVF_BOT ) 
    		{
		    if ((bot->client->pers.teamSelection == PTE_HUMANS) &&
			(bot->client->bottimeRespawn <= 0))
			NumHumansBots++;
		}
		else
		{
		    numclients++;
		    if (bot->client->pers.teamSelection != PTE_ALIENS)
			NumHumansPlayers++;
		}
	    }
	}
  
    //DEBUG
    /*char namebuff[32];
    Q_strncpyz(namebuff, self->client->pers.netname, sizeof(namebuff));
    Q_CleanStr(namebuff);
    if (!Q_stricmp(namebuff, "Soldado"))
    {
	trap_SendServerCommand(-1, va("print \"numclients: %i, NumHumansPlayers: %i, NumHumansBots: %i\n\"", 
	numclients,
	NumHumansPlayers,
	NumHumansBots
	));
    }*/

  self->botCommand = BOT_IDLE;
  self->client->bottimeRespawn = 5;
  
  if	( (numclients <= 0) || 
	((self->client->pers.teamSelection == PTE_HUMANS) && 
	((NumHumansBots+NumHumansPlayers) >= g_minPlayers.integer)) 
	)
  {
    return;
  }

  self->client->bottimeRespawn = 0;
  self->takedamage = qtrue;
    
  //if( self->client->sess.sessionTeam == TEAM_SPECTATOR ) {

    if( self->client->pers.classSelection == PTE_HUMANS ) {
    	self->client->pers.classSelection = PCL_HUMAN;
    	self->client->ps.stats[ STAT_PCLASS ] = PCL_HUMAN;
    	self->client->pers.humanItemSelection = WP_MACHINEGUN;
    	G_PushSpawnQueue( &level.humanSpawnQueue, self->client->ps.clientNum );
    	
	} else if( self->client->pers.classSelection == PTE_ALIENS) {
	  //ROTAX
	  //if (g_ambush.integer == 1)
	  //{   
	    // Es hora de evolucionar al siguiente nivel ;)
	    if ((g_ambush_stage.integer < g_ambush_maxstage.integer) && (level.time - ambush_nexttime > 0))
	    {
		ambush_next_stage();
	    }
	    else
	    {
		// NO EGGS
		// Esto hay que hacerlo bien. 
		// Deberia eliminar los EGGS porque se supone que son irrompibles (al estar en 0
		// la variable g_ambush_kill_spawns)
		/*if (g_ambush_kill_spawns.integer == 0 && ((level.time - ambush_nexttime) > 0) )
		{
		    for( i = 1, e = g_entities + i; i < level.num_entities; i++, e++ )
            	    {
            		if( e->s.modelindex == BA_A_SPAWN )
                	G_Damage( e, NULL, NULL, NULL, NULL, 10000, 0, MOD_SUICIDE );
            	    }
		}*/
		if (ROTACAK_ambush_rebuild_time_temp < level.time && ((level.time - level.startTime) > (g_ambush_sec_to_start.integer * 1000)) )
		{
  		    //srand( trap_Milliseconds( ) );
		    if (random() < 0.50f)
		    {
			self->client->ps.stats[ STAT_PCLASS ] = Bot_GetAmbushAlienClass();
			self->client->pers.classSelection = self->client->ps.stats[ STAT_PCLASS ];
      			G_PushSpawnQueue( &level.alienSpawnQueue, self->client->ps.clientNum );
  		    }
		    else // Random
		    {	
			//self->client->bottimeBuild = level.time-(int)random()*30000;
  			if (random() < 0.50f)
			{
  			    self->client->ps.stats[ STAT_PCLASS ] = PCL_ALIEN_LEVEL0;
			}
			else
			{
  			    self->client->ps.stats[ STAT_PCLASS ] = PCL_ALIEN_BUILDER0_UPG;
			}
			self->client->pers.classSelection = self->client->ps.stats[ STAT_PCLASS ];
			G_PushSpawnQueue( &level.alienSpawnQueue, self->client->ps.clientNum );
		    }
		}
	    }
	}
  //}
}



qboolean botTargetInRange( gentity_t *self, gentity_t *target) {
	trace_t   trace;
	gentity_t *traceEnt;
	vec3_t  forward, right, up;
	vec3_t  muzzle;
	AngleVectors( self->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( self, forward, right, up, muzzle );

	if( !self || !target )
		return qfalse;

  //ROTAX - niceni budov
	if( !self->client || (!target->client && g_ambush_att_buildables.integer == 0) )
		return qfalse;

	if( target->client && target->client->ps.stats[ STAT_STATE ] & SS_HOVELING )
		return qfalse;

	if( target->health <= 0 )
		return qfalse;
	
	if( self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS &&
		 self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_LEVEL3_UPG &&
		 self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_LEVEL3 &&
		 target->s.pos.trBase[2] - self->s.pos.trBase[2] > 150)
		return qfalse;
	if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS)
	{
	if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL4 && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level4_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL3_UPG && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level3UPG_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL3 && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level3_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL2_UPG && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level2UPG_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL2 && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level2_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL1_UPG && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level1UPG_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL1 && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level1_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL0 && 
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_level0_range.integer)
	{return qfalse;}
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0_UPG &&
		Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_builder_range.integer)
	{return qfalse;}
	// ALIEN_BUILDER0 NO PUEDE ATACAR
	else if(self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0)
	{return qfalse;}
	}
	else if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
	{
	    /*if((self->client->ps.stats[ STAT_PCLASS ] == PCL_HUMAN || 
		self->client->ps.stats[ STAT_PCLASS ] == PCL_HUMAN_BSUIT ) && */
	    if (Distance( self->s.pos.trBase, target->s.pos.trBase ) > g_human_range.integer)
	    {return qfalse;}
	}
	else
	{
		if(Distance( self->s.pos.trBase, target->s.pos.trBase ) > 600)
		{return qfalse;}
	}
	//BG_FindViewheightForClass(  self->client->ps.stats[ STAT_PCLASS ], &vh, NULL );
	//top[2]=vh;
	//VectorAdd( self->s.pos.trBase, top, top);
	//draw line between us and the target and see what we hit
	//trap_Trace( &trace, self->s.pos.trBase, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
	//vec3_t mins, maxs;
	//int spread = 0;

    	switch ( self->s.weapon )
    	{
    	    case WP_MACHINEGUN:
	    //spread = RIFLE_SPREAD;
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 2100 )
	        return qfalse;
	    break;

	    case WP_PAIN_SAW:
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 90 )
	        return qfalse;
	    break;

	    case WP_SHOTGUN:
	    //spread = SHOTGUN_SPREAD;
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 340 )
	        return qfalse;
	    break;

	    case WP_LAS_GUN:
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 4000 )
	        return qfalse;
	    break;

	    case WP_CHAINGUN:
	    //spread = CHAINGUN_SPREAD;
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 500 )
	        return qfalse;
	    break;

	    case WP_PULSE_RIFLE:
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 1900 )
	        return qfalse;
	    break;

	    case WP_MASS_DRIVER:
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 10000 )
	        return qfalse;
	    break;

	    case WP_FLAMER:
	    //spread = FLAMER_RADIUS;
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 295 )
	        return qfalse;
	    break;

	    case WP_LUCIFER_CANNON:
	    if( Distance( self->s.pos.trBase, target->s.pos.trBase ) > 800 )
	        return qfalse;
	    break;
	}
	
	    
	// FIXME: Hay que interpretar bien la
	// formula que simula un spread para
	// saber como utilizar esto.
	//VectorSet( mins, -spread/20, -spread/20, -spread/20 );
	//VectorSet( maxs, spread/20, spread/20, spread/20 );


	trap_Trace( &trace, muzzle, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[ trace.entityNum ];
	//if( trace.fraction < 1.0 )
	//{return qfalse;}
	// check that we hit a human and not an object
	//if( !traceEnt->client )
	//	return qfalse;
	
	//check our target is in LOS
	if(!(traceEnt == target))
		return qfalse;

	return qtrue;
}

qboolean botFriendInRange( gentity_t *self, gentity_t *target) {
	trace_t   trace;
	gentity_t *traceEnt;
	vec3_t  forward, right, up;
	vec3_t  muzzle;
	AngleVectors( self->client->ps.viewangles, forward, right, up );
	CalcMuzzlePoint( self, forward, right, up, muzzle );

	if( !self || !target )
		return qfalse;

	if( target->client && target->client->ps.stats[ STAT_STATE ] & SS_HOVELING )
		return qfalse;

	if( target->health <= 0 )
		return qfalse;
	
	trap_Trace( &trace, muzzle, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
	traceEnt = &g_entities[ trace.entityNum ];
	
	//check our target is in LOS
	if(!(traceEnt == target))
		return qfalse;

	return qtrue;
}

int botFindClosestEnemy( gentity_t *self, qboolean includeTeam ) {
	// return enemy entity index, or -1
	int vectorRange = 800; // Original: TURRET_RANGE * 3
	int i;
	int total_entities;
	int entityList[ MAX_GENTITIES ];
	vec3_t    range;
	vec3_t    mins, maxs;
	gentity_t *target, *traceEnt;
	trace_t   trace;
	int closest = -1;
	int distance = vectorRange;
	int numTelenodes = 0;
	int numEggs = 0;
	int numEnemies = 0;

	//if (g_ambush.integer == 1)//ROTAX
	distance = vectorRange = 1.0f * g_ambush_range.integer;
	
	VectorSet( range, vectorRange, vectorRange, vectorRange );
	VectorAdd( self->client->ps.origin, range, maxs );
	VectorSubtract( self->client->ps.origin, range, mins );
	
	total_entities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	
	// check list for enemies
	for( i = 0; i < total_entities; i++ ) {
		target = &g_entities[ entityList[ i ] ];
		
		if ((target->s.modelindex == BA_H_SPAWN))
		{
		    trap_Trace( &trace, self->client->ps.origin, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
		    traceEnt = &g_entities[ trace.entityNum ];
		    if (traceEnt == target)
		    {
			numTelenodes++;
		    }
		}
		else if ((target->s.modelindex == BA_A_SPAWN))
		{
		    trap_Trace( &trace, self->client->ps.origin, NULL, NULL, target->s.pos.trBase, self->s.number, MASK_SHOT );
		    traceEnt = &g_entities[ trace.entityNum ];
		    if (traceEnt == target)
		    {
			numEggs++;
		    }
		}		
		
		//ROTAX - niceni budov
		if (g_ambush_att_buildables.integer <= 0)
		{
			if (target->client && self != target && target->client->ps.stats[ STAT_PTEAM ] != self->client->ps.stats[ STAT_PTEAM ])
			{
  			// aliens ignore if it's in LOS because they have radar
  			//if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS) {
  			//	return entityList[ i ];
  			//} else {
			    int ndistance = Distance( self->s.pos.trBase, target->s.pos.trBase );
  				if( (ndistance < distance) && botTargetInRange( self, target ) ) {
  					closest = entityList[ i ];
					distance = ndistance;
					numEnemies++;
  				}
  			//}
			}
		}
		else
		{
      if ((target->client && self != target && target->client->ps.stats[ STAT_PTEAM ] != self->client->ps.stats[ STAT_PTEAM ])
      || (!target->client && self != target && self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS && self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_LEVEL0 &&
      (target->s.modelindex == BA_H_SPAWN
      || target->s.modelindex == BA_H_MGTURRET
      || target->s.modelindex == BA_H_TESLAGEN
      || target->s.modelindex == BA_H_ARMOURY
      || target->s.modelindex == BA_H_DCC
      || target->s.modelindex == BA_H_MEDISTAT
      || target->s.modelindex == BA_H_REACTOR
      || target->s.modelindex == BA_H_REPEATER)) ||
	(!target->client && self != target && self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS && (
	target->s.modelindex == BA_A_SPAWN ||
	target->s.modelindex == BA_A_BARRICADE ||
	target->s.modelindex == BA_A_BOOSTER ||
	target->s.modelindex == BA_A_ACIDTUBE ||
	target->s.modelindex == BA_A_HIVE ||
	target->s.modelindex == BA_A_TRAPPER ||
	target->s.modelindex == BA_A_OVERMIND ||
	target->s.modelindex == BA_A_HOVEL)) || (!target->client && self != target && self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL0 && (target->s.modelindex == BA_H_MGTURRET)))
      {
  			// aliens ignore if it's in LOS because they have radar
  			//if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS) {
  			//	return entityList[ i ];
  			//} else {
			    int ndistance = Distance( self->s.pos.trBase, target->s.pos.trBase );
  				if( (ndistance < distance) && botTargetInRange( self, target ) ) {
  					closest = entityList[ i ];
					distance = ndistance;
					numEnemies++;
  				}
  			//}
      }
	}
	}
	
	if(includeTeam) {
		// check list for enemies in team
		for( i = 0; i < total_entities; i++ ) {
			target = &g_entities[ entityList[ i ] ];
			
			if( target->client && self !=target && target->client->ps.stats[ STAT_PTEAM ] == self->client->ps.stats[ STAT_PTEAM ] ) {
				// aliens ignore if it's in LOS because they have radar
				//if(self->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS) {
				//	return entityList[ i ];
				//} else {
			    int ndistance = Distance( self->s.pos.trBase, target->s.pos.trBase );
  				if( (ndistance < distance) && botTargetInRange( self, target ) ) {
  					closest = entityList[ i ];
					distance = ndistance;
  				}
				//}
			}
		}
	}
	
	// Dar a conocer el camino a los amigos?
	if (self->i_APaths > 1)
	{
	    if (self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
	    {
		if ((numEnemies >= 3 || numEggs >= 1) && (level.humans_timeapaths-30000 < level.time))
		{
		    for (i = 0; i < MAX_PATHS; i++)
		    {
				level.humans_apaths[i] = self->APaths[i];
		    }
		    level.humans_timeapaths = level.time + 60000;
		}
	    }
	    else
	    {
		    if (self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_LEVEL0 ||
			self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0 ||
			self->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0_UPG)
		    {
			if (numTelenodes >= 1 && (level.aliens_timeapaths-30000 < level.time))
			{
			    for (i = 0; i < MAX_PATHS; i++)
			    {
					level.aliens_apaths[i] = self->APaths[i];
			    }
			    level.aliens_timeapaths = level.time + 140000;
			}	
		    }
		if (self->client->ps.stats[ STAT_PCLASS ] != PCL_ALIEN_LEVEL0)
		{
		    if (numEnemies >= 1 && (level.aliens_timeapaths-30000 < level.time))
		    {
			for (i = 0; i < MAX_PATHS; i++)
			{
			    level.aliens_apaths[i] = self->APaths[i];
			}
			level.aliens_timeapaths = level.time + 70000;
		    }
		}
	    }
	}
	return closest;
}

int botFindClosestFriend( gentity_t *self ) {
	// return friend entity index, or -1
	int vectorRange = 700; // Original: TURRET_RANGE * 3
	int i;
	int total_entities;
	int entityList[ MAX_GENTITIES ];
	vec3_t    range;
	vec3_t    mins, maxs;
	gentity_t *target;

	//if (g_ambush.integer == 1)//ROTAX
	//  vectorRange = 1.0f * g_ambush_range.integer;
	
	VectorSet( range, vectorRange, vectorRange, vectorRange );
	VectorAdd( self->client->ps.origin, range, maxs );
	VectorSubtract( self->client->ps.origin, range, mins );
	
	total_entities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
	
	// check list for friends
	for( i = 0; i < total_entities; i++ ) {
		target = &g_entities[ entityList[ i ] ];

		// Los humanos buscan a cualquier amigo
		if ( self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
		{
		    if ( self != target && target->client &&
		         target->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS )
		    {
			    if ( !target->botFriend )
    			    {
				if( botTargetInRange( self, target ) )
				    return entityList[ i ];			
			    }
			    else
			    {
				if ( !target->botFriend->botFriend )
				{
				    if ( target->botFriend != self )
				    {
					if( botTargetInRange( self, target ) )
					    return entityList[ i ];
				    }
				}
			    }
		    }
		}
		else
		// En cambio los aliens intentan proteger a los granger
		if ( self != target && target->client && 
		     target->client->ps.stats[ STAT_PTEAM ] == PTE_ALIENS )
		{
		    if (target->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0 || 
		        target->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0_UPG) 
		    {
			if( botTargetInRange( self, target ) )
			    return entityList[ i ];
		    }
		    else if (target->botFriend && (
		    target->botFriend->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0 || 
		    target->botFriend->client->ps.stats[ STAT_PCLASS ] == PCL_ALIEN_BUILDER0_UPG))
		    {
			if( botTargetInRange( self, target ) )
			    return entityList[ i ];
		    }    
		}		    
	}
		
	return -1;
}

// really an int? what if it's too long?
int botGetDistanceBetweenPlayer( gentity_t *self, gentity_t *player ) {
	return Distance( self->s.pos.trBase, player->s.pos.trBase );
}

qboolean botShootIfTargetInRange( gentity_t *self, gentity_t *target )
{
    trace_t	tr;
    vec3_t	forward, right, up, end, muzzle;
    gentity_t	*traceEnt;
    
    // Solo disparamos si lo tenemos en la mira
    // No es util disparar aunque este en rango si aun no esta apuntado
    AngleVectors( self->client->ps.viewangles, forward, right, up );
    CalcMuzzlePoint( self, forward, right, up, muzzle );
    VectorMA(muzzle, 8192 * 16, forward, end);
    
    trap_Trace(&tr, muzzle, target->r.mins, target->r.maxs, end, self->s.number, MASK_SHOT);
    traceEnt = &g_entities[tr.entityNum];
    
    if (traceEnt != target)
    {
	trap_Trace(&tr, muzzle, NULL, NULL, end, self->s.number, MASK_SHOT);
	traceEnt = &g_entities[tr.entityNum];
    }
    
    if (!(tr.surfaceFlags & SURF_NOIMPACT) && (traceEnt == target))
    {
	if(botTargetInRange(self,target))
	{
	  //ROTAX
			int nahoda = 0;
			//srand( trap_Milliseconds( ) );
			//nahoda = (rand() % 20);
			nahoda = (int)(( (double)rand() / ((double)(RAND_MAX)+(double)(1)) ) * 20);
			self->client->pers.cmd.buttons = 0;
			if (self->client->pers.classSelection == PCL_ALIEN_BUILDER0_UPG)//adv granger
			{
				if (nahoda > 10)
 					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				else
					self->client->pers.cmd.buttons |= BUTTON_USE_HOLDABLE;
			}
			else if (self->client->pers.classSelection == PCL_ALIEN_LEVEL1_UPG)//adv basilisk
			{
				if (nahoda > 10)
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				else
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
			}
			else if (self->client->pers.classSelection == PCL_ALIEN_LEVEL2_UPG)//adv marauder
			{
				if (nahoda < 8)
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				else
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
			}
			else if (self->client->pers.classSelection == PCL_ALIEN_LEVEL3)//dragon
			{
				if(Distance( self->s.pos.trBase, target->s.pos.trBase ) > 150 && 
					self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_SPEED)
					self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
				else
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
			}
			else if (self->client->pers.classSelection == PCL_ALIEN_LEVEL3_UPG)//adv dragon
			{
				if(self->client->ps.ammo[WP_ALEVEL3_UPG] > 0 && 
					Distance( self->s.pos.trBase, target->s.pos.trBase ) > 150 )
					self->client->pers.cmd.buttons |= BUTTON_USE_HOLDABLE;
				else
				{	
					if(Distance( self->s.pos.trBase, target->s.pos.trBase ) > 150 && 
						self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_UPG_SPEED)
						self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
					else
						self->client->pers.cmd.buttons |= BUTTON_ATTACK;
				}
			}
			else if (self->client->pers.classSelection == PCL_ALIEN_LEVEL4)//tyrant
			{
			    if(Distance( self->s.pos.trBase, target->s.pos.trBase ) > 100)
				self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
			    else
				self->client->pers.cmd.buttons |= BUTTON_ATTACK;
			}
			else//others
			{
				if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)//Human target buildable
				{
					if(self->client->ps.weapon == WP_LUCIFER_CANNON)
					{
						self->client->pers.cmd.buttons |= BUTTON_ATTACK2;
					}
					else
					{
						self->client->pers.cmd.buttons |= BUTTON_ATTACK;
					}
				}
				else
				{
					self->client->pers.cmd.buttons |= BUTTON_ATTACK;
				}
				
			}
			//if (nahoda == 15 || nahoda == 16)
        		//	self->client->pers.cmd.buttons |= BUTTON_GESTURE;
			//if (nahoda > 11 && nahoda < 15)
        		//	self->client->pers.cmd.upmove = 20;
	return qtrue;
	}
    }
    return qfalse;
}
