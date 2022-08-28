![image](https://user-images.githubusercontent.com/77735052/187063124-67d8ec6d-b488-436a-88e1-e19b2f41cf20.png)

# IrcServ, a 42 project to write an irc server in C++ (std98)

### Reference client is [Weechat](https://weechat.org/)
##### file transfers work best with weechat V2.8 - V3.5
<br/>
<br/>
adjust settings in the config.hpp
run make to compile.

```
./ircserv [port][password]
```

<br/>
<br/>

## Bonus

adjust settings in the BotConfig.hpp
run make bonus

```
./bot [serveraddr][port][password]
```

Once the bot is connected to the server, you can<br/>
write it private messages and it will use ROT13<br/>
on your text and send it back to you.<br/>
