#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <conio.h>
#include <cstdlib>
#include <random>

#include "KeyManager.h"
#include "Block.h"
#include "BlockType.h"


using namespace std;

// AI나 외부자료를 최대한 참고하지않고 테트리스게임만들기
// 1. 시간제어 while loop 와 변수를 활용해서 테스트
// 2. 키입력처리방법  (이거는 인터넷 도움을 받아야함)


// 테트리스 grid  (가로 10 x 세로 20) 
vector<vector<int>> grid;

// 현재 내리는 블록의 위치값
int curBlock_startX = 0;
int curBlock_startY = 0;

// 현재 내리는 블록
BlockType curBlockType;
int curBlockRotation = 0;

bool canSpawn = true;
int collisionNumber = 0;

int totalScore = 0;

bool gameOverFlag = false;


bool CheckBoundary(int y, int x) {

	// 유효성검사
	if (grid.empty() || grid[0].empty()) {
		return false;
	}

	return (0 <= x && x < grid[0].size() && 0 <= y && y < grid.size());
}

// BlockType 에 따라 dx, dy 포인터를 결정하는 함수
void SetBlockOffsets(BlockType type, int rotation, int*& dx, int*& dy) {
	switch (type) {
	case BlockType::BlockI:
		dx = BlockI::dx[rotation];
		dy = BlockI::dy[rotation];
		break;

	case BlockType::BlockO:
		dx = BlockO::dx[rotation];
		dy = BlockO::dy[rotation];
		break;

	case BlockType::BlockT:
		dx = BlockT::dx[rotation];
		dy = BlockT::dy[rotation];
		break;

	case BlockType::BlockS:
		dx = BlockS::dx[rotation];
		dy = BlockS::dy[rotation];
		break;

	case BlockType::BlockZ:
		dx = BlockZ::dx[rotation];
		dy = BlockZ::dy[rotation];
		break;

	case BlockType::BlockJ:
		dx = BlockJ::dx[rotation];
		dy = BlockJ::dy[rotation];
		break;

	case BlockType::BlockL:
		dx = BlockL::dx[rotation];
		dy = BlockL::dy[rotation];
		break;
	}
}



void PutBlock(BlockType type, int startX, int startY, int rotation) {
	int* dx;
	int* dy;

	SetBlockOffsets(type, rotation, dx, dy);

	// 유효성 검사 및 게임 오버 체크
	for (int i = 0; i < 4; i++) {
		int x = startX + dx[i];
		int y = startY + dy[i];

		if (!CheckBoundary(y, x)) {
			cerr << "Block이 유효범위를 벗어났음.\n";
			exit(EXIT_FAILURE);
		}

		// 게임 종료 조건 = 이미 블록이 있는 위치에 새 블록이 스폰될 때
		if (grid[y][x] == 1) {
			gameOverFlag = true;
			return;
		}
	}

	// 블록 배치
	for (int i = 0; i < 4; i++) {
		int x = startX + dx[i];
		int y = startY + dy[i];
		grid[y][x] = 1;
	}
}

// 시작 x, 시작 y 에서 block을 한 칸 내리는 함수

void ShowGrid() {
	int height = grid.size();
	int width = grid[0].size();

	for (int i = 0; i < height; i++) {
		string line;
		line.reserve(width);

		for (int j = 0; j < width; j++) {
			if (grid[i][j] == 0) {
				line += "□";  // 빈 네모
			}
			else {
				line += "■";  // 찬 네모
			}
		}

		cout << line << "\n"; // 한 줄씩 출력
	}
}



// 블록 제거될 시 위의 블록들을 끌어내린다.
void ApplyLineGravity() {
	int width = grid[0].size();
	int height = grid.size();

	vector<vector<int>> newGrid(height, vector<int>(width, 0));

	int curRow = height - 1;

	for (int i = height - 1; i >= 1; i--) {

		bool isEmptyLine = true;

		for (int j = 0; j < width; j++) {
			if (grid[i][j] == 1) {
				isEmptyLine = false;
			}
		}

		if (isEmptyLine == false) {
			for (int j = 0; j < width; j++) {
				newGrid[curRow][j] = grid[i][j];
			}

			curRow--;
		}
	}

	std::swap(grid, newGrid);
}

// 현재 블록을 왼쪽으로 한 칸 이동시키는 함수
void MoveLeft(int& curX, int& curY, BlockType type, int rotation) {
	// 이동 가능 여부 플래그
	bool moveFlag = true;

	// dx, dy 배열 가져오기
	int* dx;
	int* dy;
	SetBlockOffsets(type, rotation, dx, dy);

	vector<pair<int, int>> nextBlocks; // 다음 위치 블록
	vector<pair<int, int>> curBlocks;  // 현재 블록 위치
	map<pair<int, int>, bool> treeMap; // 현재 위치를 기록

	// 현재 블록 좌표를 저장하고 treeMap에 표시
	for (int i = 0; i < 4; i++) {
		int x = curX + dx[i];
		int y = curY + dy[i];
		curBlocks.push_back({ y, x });
		treeMap[{y, x}] = true;
	}

	// 각 블록을 왼쪽으로 이동 가능한지 확인
	for (auto& curBlock : curBlocks) {
		int y = curBlock.first;
		int x = curBlock.second;
		int nextX = x - 1; // 왼쪽 이동
		int nextY = y;

		// 경계 검사
		if (!CheckBoundary(nextY, nextX)) {
			moveFlag = false;
			break;
		}

		// 이미 다음 위치에 블록이 있는지 확인 (자기 자신 제외)
		if (treeMap.find({ nextY, nextX }) == treeMap.end() && grid[nextY][nextX] == 1) {
			moveFlag = false;
			break;
		}

		nextBlocks.push_back({ nextY, nextX });
	}

	// 이동 가능하면 기존 블록 제거 후 새로운 위치 설정
	if (moveFlag) {
		for (auto& curBlock : curBlocks) {
			grid[curBlock.first][curBlock.second] = 0;
		}

		for (auto& nextBlock : nextBlocks) {
			grid[nextBlock.first][nextBlock.second] = 1;
		}

		// 기준 좌표 갱신
		curX--;
	}
}

// 현재 블록을 오른쪽으로 한 칸 이동하는 함수
void MoveRight(int& curX, int& curY, BlockType type, int rotation) {
	bool moveFlag = true;

	// dx, dy 배열
	int* dx;
	int* dy;
	SetBlockOffsets(type, rotation, dx, dy);

	vector<pair<int, int>> nextBlocks; // 다음 위치 블록
	vector<pair<int, int>> curBlocks;  // 현재 블록 위치
	map<pair<int, int>, bool> treeMap; // 현재 위치를 기록

	// 현재 블록 좌표를 저장하고 treeMap에 표시
	for (int i = 0; i < 4; i++) {
		int x = curX + dx[i];
		int y = curY + dy[i];
		curBlocks.push_back({ y, x });
		treeMap[{y, x}] = true;
	}

	// 각 블록을 오른쪽으로 이동 가능한지 확인
	for (auto& curBlock : curBlocks) {
		int y = curBlock.first;
		int x = curBlock.second;
		int nextX = x + 1;
		int nextY = y;

		// 경계 검사
		if (!CheckBoundary(nextY, nextX)) {
			moveFlag = false;
			break;
		}

		// 이미 다음 위치에 블록이 있는지 확인 (자기 자신 제외)
		if (treeMap.find({ nextY, nextX }) == treeMap.end() && grid[nextY][nextX] == 1) {
			moveFlag = false;
			break;
		}

		nextBlocks.push_back({ nextY, nextX });
	}

	// 이동 가능하면 기존 블록 제거 후 새로운 위치 설정
	if (moveFlag) {
		for (auto& curBlock : curBlocks) {
			grid[curBlock.first][curBlock.second] = 0;
		}

		for (auto& nextBlock : nextBlocks) {
			grid[nextBlock.first][nextBlock.second] = 1;
		}

		// 기준 좌표 갱신
		curX++;
	}
}

// 일반화된 GravityBlock
void GravityBlock(BlockType type, int rotation, int& startX, int& startY) {

	bool moveFlag = true;
	vector<pair<int, int>> nextBlocks;
	vector<pair<int, int>> curBlocks;
	map<pair<int, int>, bool> treeMap;

	// 포인터 = 배열
	int* dx = nullptr;
	int* dy = nullptr;
	int blockSize = 4;

	// BlockType 에 맞는 dx, dy 배열을 설정
	SetBlockOffsets(type, rotation, dx, dy);

	// 유효성 검사
	for (int i = 0; i < 4; i++) {
		int x = startX + dx[i];
		int y = startY + dy[i];
		if (!CheckBoundary(y, x)) {
			cerr << "Block이 유효범위를 벗어났음.\n";
			exit(EXIT_FAILURE);
		}
	}


	// 먼저 현재블록들 위치를 treeMap에 true로 두고
	// 현재 블록들 위치를 vector 형태로 저장한다.
	for (int i = 0; i < blockSize; i++) {
		int curX = startX + dx[i];
		int curY = startY + dy[i];

		pair<int, int> curPosPair = make_pair(curY, curX);
		curBlocks.push_back({ curY, curX });
		treeMap[curPosPair] = true;
	}

	// 현재 블록들을 탐색하며, 다음 위치에 놓을 수 있는지를 판단한다.
	// 다음블록 판단시, 현재 블록 위치는 무시한다.
	for (auto& curBlock : curBlocks) {
		int curY = curBlock.first;
		int curX = curBlock.second;

		// y+1 한 게 nextY 이다.
		int nextY = curY + 1;
		int nextX = curX;

		// 경계검사를 한다.
		if (!CheckBoundary(nextY, nextX)) {
			moveFlag = false;
			collisionNumber++;
			return;
		}

		// 이미 nextY, nextX에 블록이 있는지 검사한다.
		// 자기자신인지도 체크한다.
		pair<int, int> nextPosPair = make_pair(nextY, nextX);

		if (treeMap.find(nextPosPair) == treeMap.end() && grid[nextY][nextX] == 1) {
			moveFlag = false;
			collisionNumber++;
			return;
		}

		nextBlocks.push_back({ nextY, nextX });
	}

	if (moveFlag) {
		for (auto& curBlock : curBlocks) {
			grid[curBlock.first][curBlock.second] = 0;
		}

		// 다음 블록 방문 처리
		for (auto& nextBlock : nextBlocks) {
			grid[nextBlock.first][nextBlock.second] = 1;
		}
	}

	// 이동처리를 해준다. & 이기에 변환된다.
	startY++;

}


void RotateClockWiseBlock(BlockType type, int& rotation, int& startX, int& startY) {

	bool moveFlag = true;
	vector<pair<int, int>> nextBlocks;
	vector<pair<int, int>> curBlocks;
	map<pair<int, int>, bool> treeMap;

	// 포인터 = 배열
	int* dx = nullptr;
	int* dy = nullptr;
	int blockSize = 4;

	// 현재 회전 상태의 오프셋 설정
	SetBlockOffsets(type, rotation, dx, dy);

	// 현재 블록 좌표 저장
	for (int i = 0; i < blockSize; i++) {
		int curX = startX + dx[i];
		int curY = startY + dy[i];

		if (!CheckBoundary(curY, curX)) {
			cerr << "Block이 유효범위를 벗어났음.\n";
			exit(EXIT_FAILURE);
		}

		pair<int, int> curPosPair = make_pair(curY, curX);
		curBlocks.push_back(curPosPair);
		treeMap[curPosPair] = true;
	}

	// 다음 회전 상태 계산
	int nextRotation = (rotation + 1) % 4;

	int* next_dx = nullptr;
	int* next_dy = nullptr;
	SetBlockOffsets(type, nextRotation, next_dx, next_dy);

	// 다음 회전 좌표 계산 및 검사
	for (int i = 0; i < blockSize; i++) {
		int nextX = startX + next_dx[i];
		int nextY = startY + next_dy[i];
		nextBlocks.push_back({ nextY, nextX });

		// 경계 검사
		if (!CheckBoundary(nextY, nextX)) {
			return; // 회전 불가
		}

		// 겹침 검사 (자기 자신 제외)
		pair<int, int> nextPos = { nextY, nextX };
		if (treeMap.find(nextPos) == treeMap.end() && grid[nextY][nextX] == 1) {
			return; // 회전 불가
		}
	}

	// ---- 여기까지 오면 회전 가능 ---- //

	// 기존 위치 비우기
	for (auto& curBlock : curBlocks) {
		grid[curBlock.first][curBlock.second] = 0;
	}

	// 새로운 위치 채우기
	for (auto& nextBlock : nextBlocks) {
		grid[nextBlock.first][nextBlock.second] = 1;
	}

	// 회전 상태 갱신
	rotation = nextRotation;
}

void RotateCounterClockWiseBlock(BlockType type, int& rotation, int& startX, int& startY) {

	bool moveFlag = true;
	vector<pair<int, int>> nextBlocks;
	vector<pair<int, int>> curBlocks;
	map<pair<int, int>, bool> treeMap;

	// 포인터 = 배열
	int* dx = nullptr;
	int* dy = nullptr;
	int blockSize = 4;

	// 현재 회전 상태의 오프셋 설정
	SetBlockOffsets(type, rotation, dx, dy);

	// 현재 블록 좌표 저장
	for (int i = 0; i < blockSize; i++) {
		int curX = startX + dx[i];
		int curY = startY + dy[i];

		if (!CheckBoundary(curY, curX)) {
			cerr << "Block이 유효범위를 벗어났음.\n";
			exit(EXIT_FAILURE);
		}

		pair<int, int> curPosPair = make_pair(curY, curX);
		curBlocks.push_back(curPosPair);
		treeMap[curPosPair] = true;
	}

	// 다음 회전 상태 계산
	int nextRotation = ((rotation - 1) + 4) % 4;

	int* next_dx = nullptr;
	int* next_dy = nullptr;
	SetBlockOffsets(type, nextRotation, next_dx, next_dy);

	// 다음 회전 좌표 계산 및 검사
	for (int i = 0; i < blockSize; i++) {
		int nextX = startX + next_dx[i];
		int nextY = startY + next_dy[i];
		nextBlocks.push_back({ nextY, nextX });

		// 경계 검사
		if (!CheckBoundary(nextY, nextX)) {
			return; // 회전 불가
		}

		// 겹침 검사 (자기 자신 제외)
		pair<int, int> nextPos = { nextY, nextX };
		if (treeMap.find(nextPos) == treeMap.end() && grid[nextY][nextX] == 1) {
			return; // 회전 불가
		}
	}

	// ---- 여기까지 오면 회전 가능 ---- //

	// 기존 위치 비우기
	for (auto& curBlock : curBlocks) {
		grid[curBlock.first][curBlock.second] = 0;
	}

	// 새로운 위치 채우기
	for (auto& nextBlock : nextBlocks) {
		grid[nextBlock.first][nextBlock.second] = 1;
	}

	// 회전 상태 갱신
	rotation = nextRotation;
}


BlockType GetRandomBlock() {
	// 난수 생성 엔진 초기화
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 6);  // 0~6 범위 난수

	int randomValue = dist(gen);
	return static_cast<BlockType>(randomValue);
}

int GetSpawnX(BlockType type) {
	switch (type) {
	case BlockType::BlockI:
		// I 블록은 폭이 4칸이므로 중앙 정렬 시 3으로 시작
		return 3;

	case BlockType::BlockO:
		// O 블록은 폭이 2칸
		return 4;

	case BlockType::BlockT:
		// T 블록 폭은 3칸
		return 3;

	case BlockType::BlockL:
		// L 블록 폭은 2칸
		return 4;

	case BlockType::BlockJ:
		// J 블록 폭은 2칸
		return 4;

	case BlockType::BlockS:
		// S 블록 폭은 3칸
		return 3;

	case BlockType::BlockZ:
		// Z 블록 폭은 3칸
		return 3;

	default:
		return 3;
	}
}

void HandleKeyInput() {
	char key = 0;

	if (_kbhit()) {        // 키가 눌렸는지 확인
		key = _getch();    // 눌린 키를 반환
	}

	// 입력이 없으면 0 반환
	if (key == 0) {
		return;
	}


	if (key == 75) {		// LeftKey
		//cout << "left key is pressed\n";
		KeyManager::GetInstance().LeftKeyPressed();
	}
	else if (key == 77) {	  // RightKey
		//cout << "right key is pressed\n";
		KeyManager::GetInstance().RightKeyPressed();
	}
	else if (key == 80) {  // DownKey
		// cout << "down key is pressed\n";
		KeyManager::GetInstance().DownKeyPressed();
	}
	else if (key == 'Q' || key == 'q') {
		//cout << "Q key is pressed\n";
		KeyManager::GetInstance().QKeyPressed();
	}
	else if (key == 'E' || key == 'e') {
		//cout << "E key is pressed\n";
		KeyManager::GetInstance().EKeyPressed();
	}
}

void ApplyKeyMove() {
	KeyManager& keyManager = KeyManager::GetInstance();

	if (keyManager.IsLeftKeyPressed()) {
		MoveLeft(curBlock_startX, curBlock_startY, curBlockType, curBlockRotation);
	}

	if (keyManager.IsRightKeyPressed()) {
		MoveRight(curBlock_startX, curBlock_startY, curBlockType, curBlockRotation);
	}

	if (keyManager.IsDownKeyPressed()) {
		// 한 칸 내리기
		GravityBlock(curBlockType, curBlockRotation, curBlock_startX, curBlock_startY);
	}

	// 회전 키 처리
	if (keyManager.IsQKeyPressed()) {
		RotateCounterClockWiseBlock(curBlockType, curBlockRotation, curBlock_startX, curBlock_startY);
	}

	if (keyManager.IsEKeyPressed()) {
		RotateClockWiseBlock(curBlockType, curBlockRotation, curBlock_startX, curBlock_startY);
	}
}

void SpawnRandomBlock() {

	curBlockType = GetRandomBlock();
	curBlockRotation = 0;
	curBlock_startX = GetSpawnX(curBlockType);
	curBlock_startY = 0;

	PutBlock(curBlockType, curBlock_startX, curBlock_startY, curBlockRotation);

	canSpawn = false;
}

void ComputeScore(int& lineClearCount) {
	switch (lineClearCount) {
	case 0:
		// 아무 줄도 제거되지 않음
		break;

	case 1:
		totalScore += 100;
		break;

	case 2:
		totalScore += 300;
		break;

	case 3:
		totalScore += 500;
		break;

	case 4:
		totalScore += 800;  // 테트리스
		break;

	default:
		// 4줄 이상은 기본적으로 4줄 처리
		totalScore += 800;
		break;
	}

	lineClearCount = 0;
}

void CheckLines() {
	bool loopFlag = true;

	int height = grid.size();
	int width = grid[0].size();

	while (loopFlag) {
		loopFlag = false;
		int lineClearCount = 0;

		// 아래에서부터 위로 검사
		for (int i = height - 1; i >= 0; i--) {
			// 한 줄 검사
			bool clearFlag = true;

			for (int j = 0; j < width; j++) {
				if (grid[i][j] == 0) {
					clearFlag = false;
				}
			}

			// 한 줄 제거
			if (clearFlag == true) {
				loopFlag = true;
				lineClearCount++;

				for (int j = 0; j < width; j++) {
					grid[i][j] = 0;
				}
			}

		}

		ComputeScore(lineClearCount);


		ApplyLineGravity();

	}
}

void ResetSpawnFlag() {

	if (collisionNumber >= 4) {

		canSpawn = true;
		collisionNumber = 0;

		CheckLines();
	}
}

void ShowTotalScore() {
	cout << "\n";
	cout << "Total Score : " << totalScore << "\n";
}

void GameLogic() {
	// 화면 클리어
	// 추후에 더블버퍼링을 해야 화면깜빡임을 없앨 수 있다.
	system("cls");

	ResetSpawnFlag();

	if (canSpawn) {
		SpawnRandomBlock();
	}

	if (gameOverFlag) {
		cout << "Game Over!\n";
		ShowTotalScore();

		return;		// 더 이상 진행하지 않고 종료
	}

	ApplyKeyMove();

	ShowGrid();
	ShowTotalScore();


	GravityBlock(curBlockType, curBlockRotation, curBlock_startX, curBlock_startY);


	// 이전 키 상태 초기화
	KeyManager::GetInstance().Reset();
}

int main() {
	// 시간제어테스트

	// while 루프 안에 while루프를 또 만들어야 게임이 돌아갈듯

	int timer = 0;
	grid.clear();
	grid.resize(20, vector<int>(10, 0));


	// 출력을 빠르게
	std::ios::sync_with_stdio(false);

	// 시간을 체크하는 루프
	while (true) {
		timer++;

		HandleKeyInput();

		if (timer >= 6000) {
			// 이 안에 게임로직을 돌려야한다.
			GameLogic();

			timer = 0;
		}
	}
}