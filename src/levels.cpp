#include "globals.h"
#include "levels.h"
#include <iostream>
#include <set>

// Set up the level
void initLevel() {

    // level = id, minX, maxX, minY, maxY, walls, enemies, pickups, interactables

    // Room A
    int roomHeight = 80;
    int pillarSep = 40;
    walls.push_back({1, "both", {1490, 1043, 0}, {1542, 1043, roomHeight}, true, true});
    walls.push_back({2, "both", {1490, 1066, 0}, {1490, 1043, roomHeight}, true, true});
    walls.push_back({3, "both", {1542, 1066, 0}, {1542, 1043, roomHeight}, true, true});
    walls.push_back({4, "both", {1542, 1066, 0}, {1584, 1066, roomHeight}, true, true});
    walls.push_back({5, "both", {1679, 1134, 0}, {1584, 1066, roomHeight}, true, true});
    walls.push_back({6, "both", {1679, 1134, 0}, {1726, 1134, roomHeight}, true, true});
    walls.push_back({8, "both", {1726, 1364, 0}, {1726, 1134, roomHeight}, true, true});
    walls.push_back({7, "both", {1726, 1619, 0}, {1726, 1364, roomHeight}, true, true});
    walls.push_back({9, "both", {1490, 1066, 0}, {1448, 1066, roomHeight}, true, true});
    walls.push_back({10, "both", {1354, 1134, 0}, {1448, 1066, roomHeight}, true, true});
    walls.push_back({11, "both", {1354, 1134, 0}, {1261, 1134, roomHeight}, true, true});
    walls.push_back({12, "both", {1630, 1619, 0}, {1448, 1619, roomHeight}, true, true});
    walls.push_back({13, "both", {1354, 1574, 0}, {1448, 1619, roomHeight}, true, true});
    walls.push_back({14, "both", {1354, 1574, 0}, {1261, 1574, roomHeight}, true, true});
    walls.push_back({15, "both", {1261, 1574, 0}, {1261, 1462, roomHeight}, true, true});
    walls.push_back({16, "both", {1261, 1272, 0}, {1261, 1134, roomHeight}, true, true});
    walls.push_back({17, "floor", {1425, 1272, -20}, {1724, 1462, -20}, false, true});
    walls.push_back({18, "floor", {1410, 1258, -10}, {1724, 1473, -10}, false, true});
    walls.push_back({19, "floor", {1400, 1248, 0}, {1724, 1482, 0}, false, true});
    walls.push_back({20, "both", {1261, 1462, 0}, {1114, 1422, roomHeight}, true, true});
    walls.push_back({21, "both", {1261, 1272, 0}, {1114, 1302, roomHeight}, true, true});
    walls.push_back({62, "both", {1630, 1619, 0}, {1658, 1876, roomHeight}, true, true});
    walls.push_back({63, "both", {1726, 1619, 0}, {1756, 1834, roomHeight}, true, true});
    int pillarWidth = 15;
    int pillarX = 1410;
    int pillarY = 1228;
    int pillarBase = 0;
    walls.push_back({200, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({201, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({202, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    walls.push_back({203, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    pillarX = 1618;
    pillarY = 1228;
    walls.push_back({204, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({205, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({206, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    walls.push_back({207, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    pillarX = 1410;
    pillarY = 1508;
    walls.push_back({208, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({209, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({210, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    walls.push_back({211, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    pillarX = 1618;
    pillarY = 1508;
    walls.push_back({212, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({213, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX+pillarWidth, pillarY-pillarWidth, roomHeight}, true, true});
    walls.push_back({214, "both", {pillarX+pillarWidth, pillarY+pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    walls.push_back({215, "both", {pillarX-pillarWidth, pillarY-pillarWidth, pillarBase}, {pillarX-pillarWidth, pillarY+pillarWidth, roomHeight}, true, true});
    pickups.push_back({1, "health", {1293, 1170, 0}});
    pickups.push_back({2, "health", {1687, 1163, 0}});
    pickups.push_back({3, "barrel", {1360, 1308, 0}});
    pickups.push_back({4, "barrel", {1690, 1357, -20}});
    pickups.push_back({5, "barrel", {1557, 1585, 0}});
    pickups.push_back({6, "corpse2", {1521, 1375, -20}});
    pickups.push_back({7, "corpse", {1509, 1587, 0}});
    pickups.push_back({8, "armor", {1287, 1536, 0}});
    pickups.push_back({9, "armor", {1464, 1598, 0}});
    pickups.push_back({10, "light", {1142, 1419, 0}});
    pickups.push_back({11, "light", {1142, 1313, 0}});
    level.push_back({1, 1189, 1735, 1000, 1658, from({1,21}, {62,63}, {200,215}), {}, from(1,11), {}});

    // K room
    int upperHeight = 60;
    roomHeight += upperHeight;
    walls.push_back({22, "both", {1076, 1202, 0}, {1114, 1302, roomHeight}, true, true});
    walls.push_back({23, "both", {1076, 1202, 0}, {842, 1202, roomHeight}, true, true});
    walls.push_back({24, "both", {704, 1294, upperHeight}, {508, 1294, roomHeight}, true, true});
    walls.push_back({25, "both", {508, 1437, upperHeight}, {508, 1294, roomHeight}, true, true});
    walls.push_back({26, "both", {508, 1437, upperHeight}, {704, 1437, roomHeight}, true, true});
    walls.push_back({27, "both", {704, 1294, roomHeight}, {794, 1248, upperHeight}, true, true});
    walls.push_back({28, "both", {794, 1248, roomHeight}, {842, 1202, 0}, true, true});
    walls.push_back({29, "both", {704, 1437, roomHeight}, {794, 1482, upperHeight}, true, true});
    walls.push_back({30, "both", {794, 1482, roomHeight}, {842, 1526, 0}, true, true});
    walls.push_back({31, "both", {842, 1526, 0}, {1076, 1526, roomHeight}, true, true});
    walls.push_back({32, "both", {1114, 1422, 0}, {1076, 1526, roomHeight}, true, true});
    walls.push_back({33, "floor", {508, 1294, upperHeight}, {704, 1437, upperHeight}, false, false});
    walls.push_back({34, "floor", {704, 1437, upperHeight, 794, 1482, upperHeight}, {842, 1391, upperHeight}, false, false});
    walls.push_back({35, "floor", {704, 1294, upperHeight, 794, 1247, upperHeight}, {842, 1338, upperHeight}, false, false});
    walls.push_back({36, "floor", {852, 1391, upperHeight}, {694, 1338, upperHeight}, false, false});
    walls.push_back({37, "line", {704, 1437, upperHeight}, {704, 1294, upperHeight}, false, true});
    walls.push_back({38, "floor", {842, 1338, upperHeight-10}, {872, 1391, upperHeight-10}, false, true});
    walls.push_back({39, "floor", {862, 1338, upperHeight-20}, {892, 1391, upperHeight-20}, false, true});
    walls.push_back({40, "floor", {882, 1338, upperHeight-30}, {912, 1391, upperHeight-30}, false, true});
    walls.push_back({41, "floor", {902, 1338, upperHeight-40}, {936, 1391, upperHeight-40}, false, true});
    walls.push_back({42, "both", {936, 1248, 0}, {980, 1248, pillarSep}, true, true});
    walls.push_back({43, "both", {980, 1294, 0}, {980, 1248, pillarSep}, true, true});
    walls.push_back({44, "both", {980, 1294, 0}, {936, 1294, pillarSep}, true, true});
    walls.push_back({45, "both", {936, 1248, 0}, {936, 1294, pillarSep}, true, true});
    walls.push_back({46, "both", {936, 1437, 0}, {980, 1437, pillarSep}, true, true});
    walls.push_back({47, "both", {980, 1482, 0}, {980, 1437, pillarSep}, true, true});
    walls.push_back({48, "both", {980, 1482, 0}, {936, 1482, pillarSep}, true, true});
    walls.push_back({49, "both", {936, 1437, 0}, {936, 1482, pillarSep}, true, true});
    walls.push_back({50, "floor", {565, 1338, upperHeight+20}, {606, 1391, upperHeight+20}, false, true});
    walls.push_back({51, "both", {794, 1248, upperHeight}, {842, 1338, 0}, true, true});
    walls.push_back({52, "both", {794, 1482, upperHeight}, {842, 1391, 0}, true, true});
    walls.push_back({53, "both", {842, 1391, upperHeight, 842, 1391, 0}, {956, 1391, 0}, false, true});
    walls.push_back({54, "both", {842, 1338, upperHeight, 842, 1338, 0}, {956, 1338, 0}, false, true});
    pickups.push_back({20, "armor", {1051, 1233, 0}});
    pickups.push_back({21, "armor", {1058, 1500, 0}});
    pickups.push_back({22, "health", {830, 1261, 0}});
    pickups.push_back({23, "health", {859, 1316, 0}});
    pickups.push_back({24, "health", {855, 1428, 0}});
    pickups.push_back({25, "health", {833, 1477, 0}});
    pickups.push_back({26, "armor", {683, 1366, upperHeight}});
    pickups.push_back({27, "armor", {649, 1366, upperHeight}});
    pickups.push_back({28, "corpse2", {628, 1325, upperHeight}});
    pickups.push_back({29, "armor2", {584, 1366, upperHeight+20}});
    pickups.push_back({30, "pillar", {957, 1272, pillarSep}});
    pickups.push_back({31, "pillar2", {957, 1272, 2*pillarSep}});
    pickups.push_back({32, "pillar", {957, 1460, pillarSep}});
    pickups.push_back({33, "pillar2", {957, 1460, 2*pillarSep}});
    level.push_back({2, 480, 1188, 1000, 1640, from({20,54}), {}, from(20,33), {}});
    roomHeight -= upperHeight;

    // A -> B corridor
    upperHeight = 40;
    roomHeight += upperHeight;
    walls.push_back({55, "both", {1630, 1619, 0}, {1658, 1876, roomHeight}, true, true});
    walls.push_back({56, "both", {1816, 1948, 0}, {1658, 1876, roomHeight}, true, true});
    walls.push_back({57, "both", {1816, 1948, 0}, {1953, 1948, roomHeight}, true, true});
    walls.push_back({58, "both", {1726, 1619, 0}, {1756, 1834, roomHeight}, true, true});
    walls.push_back({59, "both", {1821, 1857, 0}, {1756, 1834, roomHeight}, true, true});
    walls.push_back({60, "both", {1821, 1857, 0}, {1953, 1857, roomHeight}, true, true});
    walls.push_back({61, "door", {1871, 1857, 0}, {1871, 1948, roomHeight}, true, true});
    pickups.push_back({34, "corpse2", {1824, 1911, 0}});
    interactables.push_back({1, "door", 61, {1869, 1903, 0}, 60});
    level.push_back({3, 1619, 1907, 1659, 1975, from(55,61), {}, from(34,34), from({1})});
    roomHeight -= upperHeight;
    
    // Room B
    upperHeight = 40;
    roomHeight += upperHeight;
    walls.push_back({64, "both", {1960, 1857, 0}, {1960, 1719, roomHeight}, true, true});
    walls.push_back({65, "both", {2245, 1719, 0}, {1960, 1719, roomHeight}, true, true});
    walls.push_back({66, "both", {2245, 1719, 0}, {2559, 1719, roomHeight}, true, true});
    walls.push_back({67, "both", {2559, 1766, 0}, {2559, 1719, roomHeight}, true, true});
    walls.push_back({68, "both", {2559, 1857, 0}, {2559, 2181, roomHeight}, true, true});
    walls.push_back({69, "both", {2245, 2181, 0}, {2559, 2181, roomHeight}, true, true});
    walls.push_back({70, "both", {2245, 2181, 0}, {1960, 2181, roomHeight}, true, true});
    walls.push_back({71, "both", {1960, 2181, 0}, {1960, 1948, roomHeight}, true, true});
    walls.push_back({72, "both", {2281, 1846, 0}, {2430, 1846, roomHeight}, true, true, true});
    walls.push_back({73, "both", {2430, 2052, 0}, {2430, 1846, roomHeight}, true, true, true});
    walls.push_back({74, "both", {2430, 2052, 0}, {2281, 2052, roomHeight}, true, true, true});
    walls.push_back({75, "floor", {2301, 1846, 5}, {2326, 2052, 5}, false, true});
    walls.push_back({76, "floor", {2326, 1846, 10}, {2430, 2052, 10}, false, true});
    walls.push_back({77, "both", {2326, 1719, 30}, {1960, 1719, 60}, false, true});
    walls.push_back({78, "both", {1960, 1810, 30}, {1960, 1719, 60}, false, true});
    walls.push_back({79, "both", {1960, 1997, 30}, {1960, 2181, 60}, false, true});
    walls.push_back({80, "both", {2559, 2181, 30}, {1960, 2181, 60}, false, true});
    walls.push_back({81, "both", {2559, 2181, 30}, {2559, 1905, 60}, false, true});
    //int bottomHeight = roomHeight-40;
    //walls.push_back({250, "both", {2049, 1806, bottomHeight}, {2190, 1806, roomHeight}, false, true});
    //walls.push_back({251, "both", {2190, 1856, bottomHeight}, {2190, 1806, roomHeight}, false, true});
    //walls.push_back({252, "both", {2190, 1856, bottomHeight}, {2049, 1856, roomHeight}, false, true});
    //walls.push_back({253, "both", {2049, 1806, bottomHeight}, {2049, 1856, roomHeight}, false, true});
    //walls.push_back({254, "both", {2049, 2037, bottomHeight}, {2189, 2037, roomHeight}, false, true});
    //walls.push_back({255, "both", {2189, 2089, bottomHeight}, {2189, 2037, roomHeight}, false, true});
    //walls.push_back({256, "both", {2189, 2089, bottomHeight}, {2049, 2089, roomHeight}, false, true});
    //walls.push_back({257, "both", {2049, 2037, bottomHeight}, {2049, 2089, roomHeight}, false, true});
    pickups.push_back({35, "corpse3", {2363, 1782, 0}});
    pickups.push_back({36, "corpse3", {2480, 1990, 0}});
    pickups.push_back({37, "corpse", {2379, 1928, 10}});
    pickups.push_back({38, "armor", {2368, 1881, 10}});
    pickups.push_back({39, "armor", {2404, 1881, 10}});
    pickups.push_back({40, "armor", {2368, 2021, 10}});
    pickups.push_back({41, "armor", {2404, 2021, 10}});
    pickups.push_back({42, "armor", {2367, 2093, 0}});
    pickups.push_back({43, "armor", {1997, 2155, 0}});
    enemies.push_back({1, "zombieman", {2280, 1914, 0}, 90, "default"});
    enemies.push_back({2, "zombieman", {2280, 1991, 0}, 90, "cardinal"});
    //level.push_back({4, 1908, 2550, 1673, 2227, from({57},{60,61},{64,81},{82},{85},{250,257}), from({1,2}), from({35,43}), from({1})});
    level.push_back({4, 1908, 2550, 1673, 2227, from({57},{60,61},{64,81},{82},{85}), from({1,2}), from({35,43}), from({1})});
    roomHeight -= upperHeight;
    
    // B -> C corridor
    walls.push_back({82, "both", {2559, 1766, 0}, {2650, 1692, roomHeight}, true, true});
    walls.push_back({83, "both", {2742, 1692, 0}, {2650, 1692, roomHeight}, true, true});
    walls.push_back({84, "both", {2742, 1692, 0}, {2838, 1619, roomHeight}, true, true});
    walls.push_back({85, "both", {2559, 1857, 0}, {2742, 1857, roomHeight}, true, true});
    walls.push_back({86, "both", {2890, 1790, 0}, {2742, 1857, roomHeight}, true, true});
    walls.push_back({87, "both", {2890, 1790, 0}, {2958, 1626, roomHeight}, true, true});
    walls.push_back({88, "both", {2838, 1619, 0}, {2838, 1603, roomHeight}, true, true});
    pickups.push_back({44, "health", {2696, 1721, 0}});
    pickups.push_back({45, "health", {2757, 1823, 0}});
    enemies.push_back({3, "zombieman", {2884, 1645, 0}, 70, "default"});
    level.push_back({5, 2540, 2985, 1616, 1865, from({82,88}), from({3}), from({44,45}), from()});
    
    // C room 
    upperHeight = 60;
    int depth = 30;
    roomHeight += upperHeight;
    walls.push_back({ 89, "both", {2838, 1603, 0}, {2742, 1598, roomHeight}, true, true});
    walls.push_back({ 90, "both", {2742, 1456, 0}, {2742, 1598, roomHeight}, true, true});
    walls.push_back({ 91, "both", {2742, 1456, 0}, {2742, 1272, roomHeight}, true, true});
    walls.push_back({ 92, "both", {2742, 1112, 0}, {2742, 1272, roomHeight}, true, true});
    walls.push_back({ 93, "both", {2742, 1112, 0}, {2884, 1066, roomHeight}, true, true});
    walls.push_back({ 94, "both", {2958, 1619, 0}, {2958, 1579, roomHeight}, true, true});
    walls.push_back({ 95, "both", {3143, 1502, 0}, {2958, 1579, roomHeight}, true, true});
    walls.push_back({ 96, "both", {3143, 1502, 0}, {3126, 1422, roomHeight}, true, true});
    walls.push_back({ 97, "both", {3264, 1222, 0}, {3126, 1422, roomHeight}, true, true});
    walls.push_back({ 98, "both", {3264, 1222, 0}, {3252, 1162, roomHeight}, true, true});
    walls.push_back({ 99, "both", {3109, 1088, 0}, {3252, 1162, roomHeight}, true, true});
    walls.push_back({100, "both", {3109, 1088, 0}, {2973, 1066, roomHeight}, true, true});
    walls.push_back({101, "both", {3177, 1126, 0}, {3143, 1178, roomHeight-40}, true, true});
    walls.push_back({102, "both", {3264, 1239, 0}, {3143, 1178, roomHeight-40}, true, true});
    walls.push_back({103, "floor", {2725, 1482, 0}, {3143, 1632, 0}, false, false});
    walls.push_back({104, "floor", {2785, 1374, 0, 2845, 1312, 0}, {3024, 1489, 0}, false, false});
    walls.push_back({105, "floor", {2860, 1330, 0, 2895, 1404, 0}, {3126, 1302, 0}, false, false});
    walls.push_back({106, "floor", {3041, 1312, 0, 3105, 1254, 0}, {2934, 1066, 0}, false, false});
    walls.push_back({107, "floor", {2890, 1088, 0, 2924, 1166, 0}, {3154, 1066, 0}, false, false});
    walls.push_back({108, "floor", {2742, 1018, 0, 2719, 1102, 0}, {2958, 1162, 0}, false, false});
    walls.push_back({109, "floor", {2719, 1080, -depth}, {3275, 1598, -depth}, false, false});
    walls.push_back({110, "line", {3162, 1482, 0}, {3024, 1482, 0}, false, true});
    walls.push_back({150, "line", {2923, 1391, 0}, {3024, 1482, 0}, false, true});
    walls.push_back({151, "line", {2923, 1391, 0}, {3126, 1302, 0}, false, true});
    walls.push_back({152, "line", {3000, 1134, 0}, {3126, 1302, 0}, false, true});
    walls.push_back({153, "line", {3000, 1134, 0}, {3109, 1088, 0}, false, true});
    walls.push_back({154, "line", {2747, 1482, 0}, {2896, 1482, 0}, false, true});
    walls.push_back({155, "line", {2785, 1369, 0}, {2896, 1482, 0}, false, true});
    walls.push_back({156, "line", {2785, 1369, 0}, {3000, 1272, 0}, false, true});
    walls.push_back({157, "line", {2884, 1145, 0}, {3000, 1272, 0}, false, true});
    walls.push_back({158, "line", {2884, 1145, 0}, {2742, 1112, 0}, false, true});
    walls.push_back({111, "both", {2884, 975, 0}, {2884, 1066, roomHeight}, true, true});
    walls.push_back({112, "both", {2978, 975, 0}, {2973, 1066, roomHeight}, true, true});
    pickups.push_back({46, "light", {2773, 1519, 0}});
    pickups.push_back({47, "light", {3053, 1509, 0}});
    pickups.push_back({48, "shells", {2817, 1110, 0}});
    pickups.push_back({49, "corpse2", {2967, 1182, 0}});
    pickups.push_back({50, "corpse", {2830, 1533, 0}});
    enemies.push_back({4, "zombieman", {2946, 1126, 0}, 320, "stationary"});
    enemies.push_back({5, "imp", {3207, 1174, roomHeight-40}, 20, "stationary"});
    level.push_back({6, 2722, 3277, 1021, 1630, from({89,112}, {150,158}), from({4,5}), from({46,50}), from()});
    roomHeight -= upperHeight;
    
    // C -> pre-D corridor
    walls.push_back({113, "both", {2884, 975, 0}, {2884, 878, roomHeight}, true, true});
    walls.push_back({114, "both", {2884, 788, 0}, {2884, 878, roomHeight}, true, true});
    walls.push_back({115, "both", {2973, 975, 0}, {2973, 788, roomHeight}, true, true});
    walls.push_back({116, "both", {2884, 788, 0}, {2860, 694, roomHeight}, true, true});
    walls.push_back({117, "both", {3000, 694, 0}, {2973, 788, roomHeight}, true, true});
    walls.push_back({118, "door", {2884, 788, 0}, {2973, 788, roomHeight}, true, true});
    interactables.push_back({2, "door", 118, {2929, 788, 0}, 60});
    //interactables.push_back({3, "door", 113, {2884, 923, 0}, 70});
    pickups.push_back({51, "light", {2980, 745, 0}});
    pickups.push_back({52, "light", {2876, 743, 0}});
    pickups.push_back({53, "corpse3", {2958, 945, 0}});
    pickups.push_back({54, "shells", {2931, 830, 0}});
    level.push_back({7, 2862, 2998, 745, 1020, from({111,118}), from(), from({51,54}), from({2})});
    
    // pre-D room
    walls.push_back({119, "both", {2791, 716, 0}, {2821, 694, roomHeight}, true, true});
    walls.push_back({120, "both", {2791, 716, 0}, {2698, 716, roomHeight}, true, true});
    walls.push_back({121, "both", {2698, 413, 0}, {2698, 716, roomHeight}, true, true});
    walls.push_back({122, "both", {2698, 413, 0}, {2884, 413, roomHeight}, true, true});
    walls.push_back({123, "both", {2884, 370, 0}, {2884, 413, roomHeight}, true, true});
    walls.push_back({124, "both", {3000, 694, 0}, {3041, 694, roomHeight}, true, true});
    walls.push_back({125, "both", {3070, 716, 0}, {3041, 694, roomHeight}, true, true});
    walls.push_back({126, "both", {3070, 716, 0}, {3162, 716, roomHeight}, true, true});
    walls.push_back({127, "both", {3162, 413, 0}, {3162, 716, roomHeight}, true, true});
    walls.push_back({128, "both", {3162, 413, 0}, {2973, 413, roomHeight}, true, true});
    walls.push_back({129, "both", {2973, 370, 0}, {2973, 413, roomHeight}, true, true});
    walls.push_back({130, "both", {2838, 540, 0}, {2838, 694, roomHeight}, true, true});
    walls.push_back({131, "both", {3030, 540, 0}, {3030, 694, roomHeight}, true, true});
    walls.push_back({132, "both", {2860, 694, 0}, {2821, 694, roomHeight}, true, true});
    walls.push_back({133, "both", {2884, 370, 0}, {2908, 370, roomHeight}, true, true});
    walls.push_back({134, "both", {2973, 370, 0}, {2958, 370, roomHeight}, true, true});
    walls.push_back({135, "both", {2908, 320, 0}, {2908, 370, roomHeight}, true, true});
    walls.push_back({136, "both", {2958, 320, 0}, {2958, 370, roomHeight}, true, true});
    walls.push_back({137, "door", {2908, 345, 0}, {2958, 345, roomHeight}, true, true});
    interactables.push_back({4, "door", 137, {2931, 345, 0}, 60});
    pickups.push_back({55, "light2", {2855, 428, 0}});
    pickups.push_back({56, "light2", {3000, 428, 0}});
    pickups.push_back({57, "armor", {2716, 465, 0}});
    pickups.push_back({58, "health", {2716, 652, 0}});
    pickups.push_back({59, "health", {2789, 686, 0}});
    pickups.push_back({60, "clip", {2728, 697, 0}});
    pickups.push_back({61, "barrel", {2786, 641, 0}});
    pickups.push_back({62, "barrel", {2862, 568, 0}});
    pickups.push_back({63, "corpse2", {2860, 627, 0}});
    pickups.push_back({64, "health", {3143, 475, 0}});
    pickups.push_back({65, "light", {3148, 569, 0}});
    pickups.push_back({66, "barrel", {3071, 639, 0}});
    pickups.push_back({67, "health", {3073, 686, 0}});
    pickups.push_back({68, "clip", {3143, 684, 0}});
    enemies.push_back({6, "imp", {2930, 526, 0}, 1, "default"});
    level.push_back({8, 2686, 3178, 343, 773, from({116,137}), from({6}), from({51,52},{55,68}), from({4})});
    
    // D room
    walls.push_back({138, "both", {2908, 320, 0}, {2860, 320, roomHeight}, true, true});
    walls.push_back({139, "both", {2860, 181, 0}, {2860, 320, roomHeight}, true, true});
    walls.push_back({140, "both", {2860, 181, 0}, {3000, 181, roomHeight}, true, true});
    walls.push_back({141, "both", {3000, 320, 0}, {3000, 181, roomHeight}, true, true});
    walls.push_back({142, "both", {3000, 320, 0}, {2958, 320, roomHeight}, true, true});
    pickups.push_back({69, "health2", {2985, 251, 0}});
    pickups.push_back({70, "armor", {2979, 202, 0}});
    pickups.push_back({71, "armor", {2882, 202, 0}});
    interactables.push_back({5, "elevator", -1, {2860, 248, 0}, 50});
    walls.push_back({180, "button", {2860, 260, 50}, {2860, 235, 22}, false, true});
    level.push_back({9, 2833, 3013, 153, 342, from({135,142}, {180}), from(), from({69,71}), from({4,5})});
    
}

// Make sure everything is well formatted
void checkLevel() {

    // Player start angle should be between 1 and 359
    if (playerStartAngle < 1 || playerStartAngle > 359) {
        std::cerr << "Error: Player start angle should be between 1 and 359" << std::endl;
        exit(1);
    }

    // Check to make sure all the pickup names are valid
    for (auto &pickup : pickups) {
        if (!pickupNames.count(pickup.name)) {
            std::cerr << "Error: Pickup name " << pickup.name << " is not valid" << std::endl;
            exit(1);
        }
    }

    // Check to make sure the enemies have unique ids
    std::set<int> enemyIds;
    int maxEnemyId = 0;
    for (auto &enemy : enemies) {
        if (enemyIds.count(enemy.id) > 0) {
            std::cerr << "Error: Enemy id " << enemy.id << " is not unique" << std::endl;
            exit(1);
        }
        enemyIds.insert(enemy.id);
        if (enemy.id > maxEnemyId) {
            maxEnemyId = enemy.id;
        }
    }

    // Calculate the no-vision zones for each room
    for (auto &room : level) {
        int subDivisionsX = rayCastSections;
        int subDivisionsY = rayCastSections;

        // Only do it for rooms with that have blocking walls
        bool hasBlockers = false;
        for (auto &wall : walls) {
            if (wall.blockLineOfSight) {
                hasBlockers = true;
                break;
            }
        }
        if (!hasBlockers) {
            continue;
        }

        // If there aren't any enemies, no need
        if (room.enemies.size() == 0) {
            continue;
        }

        // First determine the zones 2128 1950
        room.zonesX = {};
        room.zonesY = {};
        int widthX = (room.maxX - room.minX) / subDivisionsX;
        int widthY = (room.maxY - room.minY) / subDivisionsY;
        for (int i = 0; i < subDivisionsX; i++) {
            int x1 = room.minX + i * widthX;
            int x2 = room.minX + (i + 1) * widthX - 1;
            room.zonesX.push_back({x1, x2});
            //std::cout << "zone x " << x1 << " " << x2 << std::endl;
        }
        for (int j = 0; j < subDivisionsY; j++) {
            int y1 = room.minY + j * widthY;
            int y2 = room.minY + (j + 1) * widthY - 1;
            room.zonesY.push_back({y1, y2});
            //std::cout << "zone y " << y1 << " " << y2 << std::endl;
        }

        // Ray cast from each zone to see if it can see each other zone
        room.noVisionZones = {};
        for (int ix1 = 0; ix1 < int(room.zonesX.size()); ix1++) {
            for (int iy1 = 0; iy1 < int(room.zonesY.size()); iy1++) {

                // Second wall
                for (int ix2 = 0; ix2 < int(room.zonesX.size()); ix2++) {
                    for (int iy2 = 0; iy2 < int(room.zonesY.size()); iy2++) {

                        // Not the same
                        if (ix1 == ix2 && iy1 == iy2) {
                            continue;
                        }

                        // No duplicates
                        if (ix1 > ix2 || (ix1 == ix2 && iy1 > iy2)) {
                            continue;
                        }
                        
                        // The start and end locations for the ray
                        int firstX = (room.zonesX[ix1].first + room.zonesX[ix1].second) / 2;
                        int firstY = (room.zonesY[iy1].first + room.zonesY[iy1].second) / 2;
                        int secondX = (room.zonesX[ix2].first + room.zonesX[ix2].second) / 2;
                        int secondY = (room.zonesY[iy2].first + room.zonesY[iy2].second) / 2;
                        //std::cout << "start " << firstX << " " << firstY << " end " << secondX << " " << secondY << std::endl;

                        // Ray cast
                        int currentX = firstX;
                        int currentY = firstY;
                        int numSteps = 1000;
                        int thickness = 1;
                        bool hitWall = false;
                        for (int i = 0; i < numSteps; i++) {
                            currentX = firstX + (secondX - firstX) * i / numSteps;
                            currentY = firstY + (secondY - firstY) * i / numSteps;
                            //std::cout << "current x " << currentX << " y " << currentY << std::endl;
                            for (auto &wall : walls) {
                                if (wall.blockLineOfSight) {

                                    // The start and end locations for the wall
                                    int wallX1 = wall.startLoc[0];
                                    int wallY1 = wall.startLoc[1];
                                    int wallX2 = wall.endLoc[0];
                                    int wallY2 = wall.endLoc[1];

                                    // Step 1: Compute vector AB and AP
                                    double AB_x = wallX2 - wallX1;
                                    double AB_y = wallY2 - wallY1;
                                    double AP_x = currentX - wallX1;
                                    double AP_y = currentY - wallY1;

                                    // Step 2: Project AP onto AB to find the projection length (dot product)
                                    double dotProduct = (AP_x * AB_x) + (AP_y * AB_y);
                                    double lengthAB_squared = (AB_x * AB_x) + (AB_y * AB_y);

                                    // Step 3: Find the projection point on the line
                                    double t = 0.0;
                                    if (lengthAB_squared != 0) {
                                        t = dotProduct / lengthAB_squared;
                                    }

                                    // If the projection point is on the line segment
                                    if (t >= 0 && t <= 1) {

                                        // Step 4: Compute the closest point on the line segment
                                        double closestX = wallX1 + t * AB_x;
                                        double closestY = wallY1 + t * AB_y;

                                        // Step 5: Compute the distance from the point to the closest point on the line
                                        double distance = sqrt((currentX - closestX) * (currentX - closestX) + 
                                                               (currentY - closestY) * (currentY - closestY));

                                        //std::cout << " closest " << closestX << ", " << closestY << " distance " << distance << std::endl;

                                        // If the distance is less than the thickness, we hit the wall
                                        if (distance < thickness) {
                                            hitWall = true;
                                            break;
                                        }

                                    }

                                }
                            }
                            if (hitWall) {
                                break;
                            }
                        }

                        // If we hit a wall, add the no-vision zone
                        if (hitWall) {
                            room.noVisionZones.push_back({ix1, iy1, ix2, iy2});
                            room.noVisionZones.push_back({ix2, iy2, ix1, iy1});
                            //std::cout << "No vision between " << ix1 << " " << iy1 << " and " << ix2 << " " << iy2 << std::endl;
                        }

                    }
                }
            }
        }

    }

    // Static pickups should be in the floor slightly
    for (auto &pickup : pickups) {
        if (staticPickups.count(pickup.name) > 0) {
            pickup.loc[2] -= 5;
        }
    }

    // Move each enemy to its id location
    std::vector<Enemy> newEnemies(maxEnemyId + 1);
    for (auto &enemy : enemies) {
        newEnemies[enemy.id] = enemy;
    }
    enemies = newEnemies;

    // Check to make sure the walls have unique ids
    std::set<int> wallIds;
    int maxWallId = 0;
    for (auto &wall : walls) {
        if (wallIds.count(wall.id) > 0) {
            std::cerr << "Error: Wall id " << wall.id << " is not unique" << std::endl;
            exit(1);
        }
        wallIds.insert(wall.id);
        if (wall.id > maxWallId) {
            maxWallId = wall.id;
        }
    }

    // Move each wall to its id location
    std::vector<Wall> newWalls(maxWallId + 1);
    for (auto &wall : walls) {
        newWalls[wall.id] = wall;
    }
    walls = newWalls;

    // Check to make sure the pickups have unique ids
    std::set<int> pickupIds;
    int maxPickupId = 0;
    for (auto &pickup : pickups) {
        if (pickupIds.count(pickup.id) > 0) {
            std::cerr << "Error: Pickup id " << pickup.id << " is not unique" << std::endl;
            exit(1);
        }
        pickupIds.insert(pickup.id);
        if (pickup.id > maxPickupId) {
            maxPickupId = pickup.id;
        }
    }

    // Move each pickup to its id location
    std::vector<Pickup> newPickups(maxPickupId + 1);
    for (auto &pickup : pickups) {
        newPickups[pickup.id] = pickup;
    }
    pickups = newPickups;

    // Check to make sure the rooms have unique ids
    std::set<int> roomIds;
    int maxRoomId = 0;
    for (auto &room : level) {
        if (roomIds.count(room.id) > 0) {
            std::cerr << "Error: Room id " << room.id << " is not unique" << std::endl;
            exit(1);
        }
        roomIds.insert(room.id);
        if (room.id > maxRoomId) {
            maxRoomId = room.id;
        }
    }

    // Check to make sure the interactables have unique ids
    std::set<int> interactableIds;
    int maxInteractableId = 0;
    for (auto &interactable : interactables) {
        if (interactableIds.count(interactable.id) > 0) {
            std::cerr << "Error: Interactable id " << interactable.id << " is not unique" << std::endl;
            exit(1);
        }
        interactableIds.insert(interactable.id);
        if (interactable.id > maxInteractableId) {
            maxInteractableId = interactable.id;
        }
    }

    // Move each interactable to its id location
    std::vector<Interactable> newInteractables(maxInteractableId + 1);
    for (auto &interactable : interactables) {
        newInteractables[interactable.id] = interactable;
    }
    interactables = newInteractables;

    // Make sure each reference is valid
    for (auto &room : level) {
        for (auto &wall : room.walls) {
            if (wallIds.count(wall) == 0) {
                std::cerr << "Error: Wall id " << wall << " is not valid" << std::endl;
                exit(1);
            }
        }
        for (auto &enemy : room.enemies) {
            if (enemyIds.count(enemy) == 0) {
                std::cerr << "Error: Enemy id " << enemy << " is not valid" << std::endl;
                exit(1);
            }
        }
        for (auto &pickup : room.pickups) {
            if (pickupIds.count(pickup) == 0) {
                std::cerr << "Error: Pickup id " << pickup << " is not valid" << std::endl;
                exit(1);
            }
        }
        for (auto &interactable : room.interactables) {
            if (interactableIds.count(interactable) == 0) {
                std::cerr << "Error: Interactable id " << interactable << " is not valid" << std::endl;
                exit(1);
            }
        }
    }

    // Walls should have the start loc as the top
    for (auto &wall : walls) {
        if (wall.startLoc.size() > 0) {
            if (wall.startLoc[2] < wall.endLoc[2]) {
                int temp = wall.startLoc[2];
                wall.startLoc[2] = wall.endLoc[2];
                wall.endLoc[2] = temp;
            }
        }
    }

    // Walls should have the top-left as the start loc
    for (auto &wall : walls) {
        if (wall.startLoc.size() > 0 && wall.type != "floor") {
            if (wall.startLoc[0] > wall.endLoc[0] || (wall.startLoc[0] == wall.endLoc[0] && wall.startLoc[1] > wall.endLoc[1])) {

                // Swap the x
                int temp = wall.startLoc[0];
                wall.startLoc[0] = wall.endLoc[0];
                wall.endLoc[0] = temp;

                // Swap the y
                temp = wall.startLoc[1];
                wall.startLoc[1] = wall.endLoc[1];
                wall.endLoc[1] = temp;

            }
        }
    }

    // If two walls have the same loc
    for (auto &wall1 : walls) {
        if (wall1.startLoc.size() == 0) {
            continue;
        }
        for (auto &wall2 : walls) {
            if (wall2.startLoc.size() == 0) {
                continue;
            }
            std::vector<int> startLoc1 = {wall1.startLoc[0], wall1.startLoc[1], 0};
            std::vector<int> endLoc1 = {wall1.endLoc[0], wall1.endLoc[1], 0};
            std::vector<int> startLoc2 = {wall2.startLoc[0], wall2.startLoc[1], 0};
            std::vector<int> endLoc2 = {wall2.endLoc[0], wall2.endLoc[1], 0};
            int height1 = std::max(wall1.startLoc[2], wall1.endLoc[2]);
            int height2 = std::max(wall2.startLoc[2], wall2.endLoc[2]);
            if (wall1.id != wall2.id && height1 == height2) {
                if (endLoc1 == startLoc2 && wall1.type == "both" && (wall2.type == "left" || wall2.type == "both")) {
                    wall1.type = "left";
                }
                if (endLoc1 == endLoc2 && wall1.type == "both" && (wall2.type == "right" || wall2.type == "both")) {
                    wall1.type = "left";
                }
                if (startLoc1 == startLoc2 && wall1.type == "both" && (wall2.type == "left" || wall2.type == "both")) {
                    wall1.type = "right";
                }
                if (startLoc1 == endLoc2 && wall1.type == "both" && (wall2.type == "right" || wall2.type == "both")) {
                    wall1.type = "right";
                }
            }
        }
    }

    // Output all walls
    for (auto &wall : walls) {
        if (wall.startLoc.size() > 0) {
            std::cout << "Wall " << wall.id << " " << wall.type << " " << wall.startLoc[0] << " " << wall.startLoc[1] << " " << wall.startLoc[2] << " " << wall.endLoc[0] << " " << wall.endLoc[1] << " " << wall.endLoc[2] << std::endl;
        }
    }

    // Check for any walls with similar x or y coords
    int checkDistance = 5;
    for (auto &wall1 : walls) {
        if (wall1.startLoc.size() == 0) {
            continue;
        }
        for (auto &wall2 : walls) {
            if (wall2.startLoc.size() == 0) {
                continue;
            }
            if (wall1.id < wall2.id) {

                // Each of the x checks
                if (std::abs(wall1.startLoc[0] - wall2.startLoc[0]) < checkDistance && wall1.startLoc[0] != wall2.startLoc[0]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar x coords: " << wall1.startLoc[0] << " vs " << wall2.startLoc[0] << std::endl;
                } else if (std::abs(wall1.startLoc[0] - wall2.endLoc[0]) < checkDistance && wall1.startLoc[0] != wall2.endLoc[0]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar x coords: " << wall1.startLoc[0] << " vs " << wall2.endLoc[0] << std::endl;
                } else if (std::abs(wall1.endLoc[0] - wall2.endLoc[0]) < checkDistance && wall1.endLoc[0] != wall2.endLoc[0]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar x coords: " << wall1.endLoc[0] << " vs " << wall2.endLoc[0] << std::endl;
                } else if (std::abs(wall1.endLoc[0] - wall2.startLoc[0]) < checkDistance && wall1.endLoc[0] != wall2.startLoc[0]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar x coords: " << wall1.endLoc[0] << " vs " << wall2.startLoc[0] << std::endl;
                }

                // Each of the y checks
                if (std::abs(wall1.startLoc[1] - wall2.startLoc[1]) < checkDistance && wall1.startLoc[1] != wall2.startLoc[1]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar y coords: " << wall1.startLoc[1] << " vs " << wall2.startLoc[1] << std::endl;
                } else if (std::abs(wall1.startLoc[1] - wall2.endLoc[1]) < checkDistance && wall1.startLoc[1] != wall2.endLoc[1]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar y coords: " << wall1.startLoc[1] << " vs " << wall2.endLoc[1] << std::endl;
                } else if (std::abs(wall1.endLoc[1] - wall2.endLoc[1]) < checkDistance && wall1.endLoc[1] != wall2.endLoc[1]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar y coords: " << wall1.endLoc[1] << " vs " << wall2.endLoc[1] << std::endl;
                } else if (std::abs(wall1.endLoc[1] - wall2.startLoc[1]) < checkDistance && wall1.endLoc[1] != wall2.startLoc[1]) {
                    std::cout << "Warning: Wall " << wall1.id << " and wall " << wall2.id << " have similar y coords: " << wall1.endLoc[1] << " vs " << wall2.startLoc[1] << std::endl;
                }

            }
        }
    }

}

// Utility - return a vector from a to b
std::vector<int> from(int a, int b) {
    std::vector<int> result = {};
    for (int i = a; i <= b; i++) {
        result.push_back(i);
    }
    return result;
}
std::vector<int> from(std::vector<int> a, std::vector<int> b, std::vector<int> c, std::vector<int> d, std::vector<int> e, std::vector<int> f, std::vector<int> g, std::vector<int> h) {
    std::vector<int> result = {};
    if (a.size() >= 2) {
        for (int i : from(a[0], a[1])) {
            result.push_back(i);
        }
    } else if (a.size() == 1) {
        result.push_back(a[0]);
    }
    if (b.size() >= 2) {
        for (int i : from(b[0], b[1])) {
            result.push_back(i);
        }
    } else if (b.size() == 1) {
        result.push_back(b[0]);
    }
    if (c.size() >= 2) {
        for (int i : from(c[0], c[1])) {
            result.push_back(i);
        }
    } else if (c.size() == 1) {
        result.push_back(c[0]);
    }
    if (d.size() >= 2) {
        for (int i : from(d[0], d[1])) {
            result.push_back(i);
        }
    } else if (d.size() == 1) {
        result.push_back(d[0]);
    }
    if (e.size() >= 2) {
        for (int i : from(e[0], e[1])) {
            result.push_back(i);
        }
    } else if (e.size() == 1) {
        result.push_back(e[0]);
    }
    if (f.size() >= 2) {
        for (int i : from(f[0], f[1])) {
            result.push_back(i);
        }
    } else if (f.size() == 1) {
        result.push_back(f[0]);
    }
    if (g.size() >= 2) {
        for (int i : from(g[0], g[1])) {
            result.push_back(i);
        }
    } else if (g.size() == 1) {
        result.push_back(g[0]);
    }
    if (h.size() >= 2) {
        for (int i : from(h[0], h[1])) {
            result.push_back(i);
        }
    } else if (h.size() == 1) {
        result.push_back(h[0]);
    }
    return result;
}


