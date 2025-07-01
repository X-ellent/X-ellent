.welstr "Welcome to the Xellent terminal network"
.logstr "Login from terminal at "
.userstr "User "
.basestr "Based at "
.colon ":"
.dash ","
.xlis "XLNT Information Systems: "
.opt "Please select your desired option:"
.elo  "  ESC Logout."
.emm  "  ESC Main Menu."
.inv "Invalid option! Choose one of:"
.prompt "->"
.xx "xxxxx"
.hitakey "Hit a key."
.moretx "<More>"
.pltitle "  User     Deaths Kills Turrets Rating Score"
.money "      You have $"
.whotxt "Get status of which player:"
.notfnd "No such player '"
.afford "You cannot Afford that service"
.quote "'"
.txtinfo   "Information for user "
.txtname   "Name     : "
.txtscore  "Score    : "
.txtrating "Rating   : "
.txtcash   "Cash     : "
.txtfuel   "Fuel     : "
.txtshield "Shield   : "
.txtlocat  "Location : "


.return
RTS

.start
CLS THI
welstr STR NEW TLO
logstr STR THI 68 FET 64 FET 60 FET NUM colon STR NUM dash STR NUM TLO NEW
userstr STR THI 0 STR SPC TLO
basestr STR THI 88 FET 84 FET 80 FET NUM colon STR NUM dash STR NUM TLO NEW
mainmenu JSR
RTS

.mmtxt "Main Menu"
.mm1   "   1  Map Information."
.mm2   "   2  Player Information."
.mm3  "   3  Banking Services."
.mainmenu
NEW NEW THI mmtxt xlis STR STR NEW opt STR NEW
.mmloop
TLO mm1 STR NEW mm2 STR NEW mm3 STR NEW elo STR NEW prompt STR INN
POPA
1 CMPA mapmenu BEQ
2 CMPA playmenu BEQ
3 CMPA bankmenu BEQ
-1 CMPA return BEQ
inv NEW THI STR NEW 
mmloop JMP

.mptxt "Map Menu"
.mp1  "   1  View Level Map."
.mp2  "   2  Lift Information."
.mapmenu
NEW NEW THI mptxt xlis STR STR NEW opt STR NEW
.mploop
TLO mp1 STR NEW mp2 STR NEW emm STR NEW prompt STR INN
POPA
1 CMPA showlevel BEQ
2 CMPA liftmenu BEQ
-1 CMPA mainmenu BEQ
inv NEW THI STR NEW 
mploop JMP

.showlevel
60 FET 0 SYSTEM
KEY POP mapmenu JMP

.rebroad "rebroad"

.pmtxt "Player Menu"
.pm1  "   1  View Player List"
.pm2  "   2  Buy Player Information ($50)"
.pm9  "   9  Extended options."
.playmenu
NEW NEW THI pmtxt xlis STR STR NEW opt STR NEW
.pmloop
TLO pm1 STR NEW pm2 STR NEW
rebroad 0 6 SYSTEM 0 CMP isnotme BNE
pm9 STR NEW
.isnotme
POP
emm STR NEW THI money STR 56 FET NUM NEW TLO
prompt STR INN
POPA
1 CMPA playlist BEQ
2 CMPA playinfo BEQ
rebroad 0 6 SYSTEM 0 CMP isnotmeb BNE
8 CMPA extended BEQ
.isnotmeb
POP
-1 CMPA mainmenu BEQ
inv NEW THI STR NEW 
pmloop JMP

.playlist
NEW NEW THI pltitle STR NEW TLO
0 POPA 2 SYSTEM POPB
.doloop
128 PSHB 1 SYSTEM 
SPC SPC 128 STR 12 TAB 220 FET NUM 18 TAB 224 FET NUM 25 TAB 228 FET NUM
33 TAB 176 FET NUM 39 TAB 180 FET NUM
NEW
PSHB 3 SYSTEM POPB PSHB 0 CMP exitpl BEQ POP
INCA 20 CMPA morepl BEQ
doloop JMP
.morepl
THI 0 POPA moretx STR KEY POP NEW NEW
pltitle STR NEW TLO
doloop JMP
.exitpl
POP
THI hitakey STR
KEY POP
playmenu JMP

.playinfo
50 56 FET CMP nocash BMI POP
NEW NEW THI whotxt STR TLO 256 16 INS NEW
256 4 SYSTEM 0 CMP noplayer BEQ
128 SWAP 1 SYSTEM
50 5 SYSTEM
THI txtinfo STR TLO 128 STR NEW
THI txtname STR TLO 144 STR NEW
THI txtscore STR TLO 180 FET NUM NEW
THI txtrating STR TLO 176 FET NUM NEW
THI txtcash STR TLO 184 FET NUM NEW
THI txtfuel STR TLO 200 FET NUM NEW
THI txtshield STR TLO 204 FET NUM
playmenu JMP
.noplayer
THI notfnd STR TLO 256 STR THI quote STR NEW
playmenu JMP
.nocash POP
NEW THI afford STR TLO playmenu JMP

.xttxt "Extension"
.xt1  "   1  Set money."
.extended
NEW NEW THI xttxt xlis STR STR NEW opt STR NEW
.xtloop
TLO xt1 STR NEW emm STR NEW prompt STR INN
POPA
1 CMPA gaincash BEQ
-1 CMPA playmenu BEQ
inv NEW THI STR NEW 
xtloop JMP

.howmuch "How much money do you want:"
.gaincash
NEW THI howmuch STR TLO 256 16 INS NEW
56 FET 5 SYSTEM 256 8 SYSTEM NEG 5 SYSTEM 
THI money STR 56 FET NUM NEW TLO
extended JMP

.lmtxt "Lift Menu"
.lm1  "   1  Get Lift Information."
.liftmenu
NEW NEW THI lmtxt xlis STR STR NEW opt STR NEW
.lmloop
TLO lm1 STR NEW emm STR NEW prompt STR INN
POPA
1 CMPA liftinfo BEQ
-1 CMPA mapmenu BEQ
inv NEW THI STR NEW 
lmloop JMP

.liquery "Enter lift ID number:"
.linot "There is no lift with that ID number"
.liid    "Id Number   : "
.lilev   "Level       : "
.licoord "Coordinates : "
.litarg  "Target Lev. : "
.lipass  "Password    : "
.liftinfo
NEW NEW THI liquery STR TLO 256 9 INS NEW
256 8 SYSTEM DUP POPA 128 13 SYSTEM notlift BNE
THI liid STR TLO 128 FET NUM NEW
THI lilev STR TLO 132 FET NUM NEW
THI licoord STR TLO 136 FET NUM dash STR 140 FET NUM NEW
THI litarg STR TLO 144 FET NUM NEW
NEW
liftmenu JMP
.notlift
THI linot STR NEW TLO
liftmenu JMP

.bktxt "Banking Services"
.bk1  "   1  Transfer Money."
.bankmenu
NEW NEW THI bktxt xlis STR STR NEW opt STR NEW
.bkloop
TLO bk1 STR NEW emm STR NEW
THI money STR 56 FET NUM NEW TLO
 prompt STR INN POPA
1 CMPA cashtrans BEQ
-1 CMPA mainmenu BEQ
inv NEW THI STR NEW 
bkloop JMP

.credtxt "Transfer of credit (5% charge)..."
.ammtxt "Enter Amount :"
.insuftxt "Insufficient funds."
.whobtxt "Credit who's account :"
.oktxt "Transaction completed."
.cashtrans
THI NEW credtxt STR NEW
ammtxt STR TLO 256 16 INS NEW 256 8 SYSTEM POPB
56 FET CMPB notenuf BMI
0 CMPB notrans BEQ
THI whobtxt STR TLO 256 16 INS NEW 256 4 SYSTEM
POPA 0 CMPA notaccnt BEQ
PSHB PSHA 12 SYSTEM
PSHB 5 SYSTEM
THI oktxt STR NEW TLO
bankmenu JMP
.notenuf
THI insuftxt STR .notrans NEW bankmenu JMP
.notaccnt
THI notfnd STR TLO 256 STR THI quote STR NEW bankmenu JMP
