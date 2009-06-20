void CreateMissile(int x, int y);
void HandleMissile();
void DrawMissile();
void UndrawMissile();
void MissileCollision(int x, int y, int rad);
void DestroyTarget(int TargetNum, int TargetX);
int Split(int MissileNum);

void UndrawFlyer(void);
void HandleFlyer(void);
void DrawFlyer(void);
void CreateFlyer(void);

extern int MissileCount,FlyerCount;
