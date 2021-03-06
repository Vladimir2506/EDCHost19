#pragma once

#include "stdafx.h"

class MainLogic final
{
private:
	static MainLogic * pInstance;
	MatchInfo theMatch;
	MainLogic();
	~MainLogic();
public:
	static MainLogic *GetInstance();
	static void DestroyInstance();
	void MatchBegin();
	void MatchEnd();
	void MatchPause();
	void MatchResume();
	void Run(const CameraInfo &pts);
	void ShootOut(int nSide);
	void PlusOne(int nSide);
	void Penalty(int nSide);
	void ResetInfo();
	void EvaluateEvil();
	void ResetEvil(int nSide);
	MatchInfo GetInfo();
};