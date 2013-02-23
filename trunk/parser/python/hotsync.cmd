/* REXX */

'@ECHO OFF'

CALL RxFuncAdd 'SysCls', 'RexxUtil', 'SysCls'
CALL rxfuncadd 'sysloadfuncs', 'rexxutil', 'sysloadfuncs'
CALL sysloadfuncs

'cls'
say "\\ Archiving old /cache..."

/* This would be a way to archive your old databases: 
 * Note: Adjust the absolute path!
 *
 * 'zip -m F:\PalmPilot\Plucker\DB_'|| Date('S') ||'.zip .\.plucker\cache\*'
 */
 
say "// Rebuilding Plucker database..."

'set home=.'

/* Remove old files. You don't need this if you use ZIP like above:
 * Note: For usage with various databases add something like this for
 * each directory you use.
 */
 
rc=Directory("./.plucker/html/*")
'del * /N'
rc=Directory("./plucker/html*")
'del * /N'

rc=Directory("E:/PalmPilot/Plucker/parser/python")

/* Use this to create a specific database only on monday:
 *
 * if Date('W')='Monday' then
 * do
 * 'zip -m F:\PalmPilot\Plucker\Weekly_'|| Date('S') ||'.zip .\.weekly\cache\*'
 * 
 * 'python ./PyPlucker/spider.py -c -V1 -p.weekly'
 * end
 */

/* Call for usage with 2bit colour:
 * 'python ./PyPlucker/spider.py -c -V1 -p.shedules --bpp=2'
 */

/* Normal call for the spider: */
'python ./PyPlucker/spider.py -c -V1 -p.daily'

/* If you want to sync in more than one database use the follwoing
 * syntax:
 *
 * 'perl ../../conduit/sync.pl --name="WeeklyDB PluckerDB" --cache=".weekly  
 * .daily"';
 */
 
/* Call for the conduit: */ 
'perl ../../conduit/sync.pl --name="PluckerDB" --cache=".daily"';
 

 
