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

// AI�� �ܺ��ڷḦ �ִ��� ���������ʰ� ��Ʈ�������Ӹ����
// 1. �ð����� while loop �� ������ Ȱ���ؼ� �׽�Ʈ
// 2. Ű�Է�ó�����  (�̰Ŵ� ���ͳ� ������ �޾ƾ���)


// ��Ʈ���� grid  (���� 10 x ���� 20) 
vector<vector<int>> grid;

// ���� ������ ����� ��ġ��
int curBlock_startX = 0;
int curBlock_startY = 0;

// ���� ������ ���
BlockType curBlockType;
int curBlockRotation = 0;

bool canSpawn = true;
int collisionNumber = 0;

int totalScore = 0;

bool gameOverFlag = false;


bool CheckBoundary(int y, int x) {

	// ��ȿ���˻�
	if (grid.empty() || grid[0].empty()) {
		return false;
	}

	return (0 <= x && x < grid[0].size() && 0 <= y && y < grid.size());
}

// BlockType �� ���� dx, dy �����͸� �����ϴ� �Լ�
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

	// ��ȿ�� �˻� �� ���� ���� üũ
	for (int i = 0; i < 4; i++) {
		int x = startX + dx[i];
		int y = startY + dy[i];

		if (!CheckBoundary(y, x)) {
			cerr << "Block�� ��ȿ������ �����.\n";
			exit(EXIT_FAILURE);
		}

		// ���� ���� ���� = �̹� ����� �ִ� ��ġ�� �� ����� ������ ��
		if (grid[y][x] == 1) {
			gameOverFlag = true;
			return;
		}
	}

	// ��� ��ġ
	for (int i = 0; i < 4; i++) {
		int x = startX + dx[i];
		int y = startY + dy[i];
		grid[y][x] = 1;
	}
}

// ���� x, ���� y ���� block�� �� ĭ ������ �Լ�

void ShowGrid() {
	int height = grid.size();
	int width = grid[0].size();

	for (int i = 0; i < height; i++) {
		string line;
		line.reserve(width);

		for (int j = 0; j < width; j++) {
			if (grid[i][j] == 0) {
				line += "��";  // �� �׸�
			}
			else {
				line += "��";  // �� �׸�
			}
		}

		cout << line << "\n"; // �� �پ� ���
	}
}



// ��� ���ŵ� �� ���� ��ϵ��� �������.
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

// ���� ����� �������� �� ĭ �̵���Ű�� �Լ�
void MoveLeft(int& curX, int& curY, BlockType type, int rotation) {
	// �̵� ���� ���� �÷���
	bool moveFlag = true;

	// dx, dy �迭 ��������
	int* dx;
	int* dy;
	SetBlockOffsets(type, rotation, dx, dy);

	vector<pair<int, int>> nextBlocks; // ���� ��ġ ���
	vector<pair<int, int>> curBlocks;  // ���� ��� ��ġ
	map<pair<int, int>, bool> treeMap; // ���� ��ġ�� ���

	// ���� ��� ��ǥ�� �����ϰ� treeMap�� ǥ��
	for (int i = 0; i < 4; i++) {
		int x = curX + dx[i];
		int y = curY + dy[i];
		curBlocks.push_back({ y, x });
		treeMap[{y, x}] = true;
	}

	// �� ����� �������� �̵� �������� Ȯ��
	for (auto& curBlock : curBlocks) {
		int y = curBlock.first;
		int x = curBlock.second;
		int nextX = x - 1; // ���� �̵�
		int nextY = y;

		// ��� �˻�
		if (!CheckBoundary(nextY, nextX)) {
			moveFlag = false;
			break;
		}

		// �̹� ���� ��ġ�� ����� �ִ��� Ȯ�� (�ڱ� �ڽ� ����)
		if (treeMap.find({ nextY, nextX }) == treeMap.end() && grid[nextY][nextX] == 1) {
			moveFlag = false;
			break;
		}

		nextBlocks.push_back({ nextY, nextX });
	}

	// �̵� �����ϸ� ���� ��� ���� �� ���ο� ��ġ ����
	if (moveFlag) {
		for (auto& curBlock : curBlocks) {
			grid[curBlock.first][curBlock.second] = 0;
		}

		for (auto& nextBlock : nextBlocks) {
			grid[nextBlock.first][nextBlock.second] = 1;
		}

		// ���� ��ǥ ����
		curX--;
	}
}

// ���� ����� ���������� �� ĭ �̵��ϴ� �Լ�
void MoveRight(int& curX, int& curY, BlockType type, int rotation) {
	bool moveFlag = true;

	// dx, dy �迭
	int* dx;
	int* dy;
	SetBlockOffsets(type, rotation, dx, dy);

	vector<pair<int, int>> nextBlocks; // ���� ��ġ ���
	vector<pair<int, int>> curBlocks;  // ���� ��� ��ġ
	map<pair<int, int>, bool> treeMap; // ���� ��ġ�� ���

	// ���� ��� ��ǥ�� �����ϰ� treeMap�� ǥ��
	for (int i = 0; i < 4; i++) {
		int x = curX + dx[i];
		int y = curY + dy[i];
		curBlocks.push_back({ y, x });
		treeMap[{y, x}] = true;
	}

	// �� ����� ���������� �̵� �������� Ȯ��
	for (auto& curBlock : curBlocks) {
		int y = curBlock.first;
		int x = curBlock.second;
		int nextX = x + 1;
		int nextY = y;

		// ��� �˻�
		if (!CheckBoundary(nextY, nextX)) {
			moveFlag = false;
			break;
		}

		// �̹� ���� ��ġ�� ����� �ִ��� Ȯ�� (�ڱ� �ڽ� ����)
		if (treeMap.find({ nextY, nextX }) == treeMap.end() && grid[nextY][nextX] == 1) {
			moveFlag = false;
			break;
		}

		nextBlocks.push_back({ nextY, nextX });
	}

	// �̵� �����ϸ� ���� ��� ���� �� ���ο� ��ġ ����
	if (moveFlag) {
		for (auto& curBlock : curBlocks) {
			grid[curBlock.first][curBlock.second] = 0;
		}

		for (auto& nextBlock : nextBlocks) {
			grid[nextBlock.first][nextBlock.second] = 1;
		}

		// ���� ��ǥ ����
		curX++;
	}
}

// �Ϲ�ȭ�� GravityBlock
void GravityBlock(BlockType type, int rotation, int& startX, int& startY) {

	bool moveFlag = true;
	vector<pair<int, int>> nextBlocks;
	vector<pair<int, int>> curBlocks;
	map<pair<int, int>, bool> treeMap;

	// ������ = �迭
	int* dx = nullptr;
	int* dy = nullptr;
	int blockSize = 4;

	// BlockType �� �´� dx, dy �迭�� ����
	SetBlockOffsets(type, rotation, dx, dy);

	// ��ȿ�� �˻�
	for (int i = 0; i < 4; i++) {
		int x = startX + dx[i];
		int y = startY + dy[i];
		if (!CheckBoundary(y, x)) {
			cerr << "Block�� ��ȿ������ �����.\n";
			exit(EXIT_FAILURE);
		}
	}


	// ���� �����ϵ� ��ġ�� treeMap�� true�� �ΰ�
	// ���� ��ϵ� ��ġ�� vector ���·� �����Ѵ�.
	for (int i = 0; i < blockSize; i++) {
		int curX = startX + dx[i];
		int curY = startY + dy[i];

		pair<int, int> curPosPair = make_pair(curY, curX);
		curBlocks.push_back({ curY, curX });
		treeMap[curPosPair] = true;
	}

	// ���� ��ϵ��� Ž���ϸ�, ���� ��ġ�� ���� �� �ִ����� �Ǵ��Ѵ�.
	// ������� �Ǵܽ�, ���� ��� ��ġ�� �����Ѵ�.
	for (auto& curBlock : curBlocks) {
		int curY = curBlock.first;
		int curX = curBlock.second;

		// y+1 �� �� nextY �̴�.
		int nextY = curY + 1;
		int nextX = curX;

		// ���˻縦 �Ѵ�.
		if (!CheckBoundary(nextY, nextX)) {
			moveFlag = false;
			collisionNumber++;
			return;
		}

		// �̹� nextY, nextX�� ����� �ִ��� �˻��Ѵ�.
		// �ڱ��ڽ������� üũ�Ѵ�.
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

		// ���� ��� �湮 ó��
		for (auto& nextBlock : nextBlocks) {
			grid[nextBlock.first][nextBlock.second] = 1;
		}
	}

	// �̵�ó���� ���ش�. & �̱⿡ ��ȯ�ȴ�.
	startY++;

}


void RotateClockWiseBlock(BlockType type, int& rotation, int& startX, int& startY) {

	bool moveFlag = true;
	vector<pair<int, int>> nextBlocks;
	vector<pair<int, int>> curBlocks;
	map<pair<int, int>, bool> treeMap;

	// ������ = �迭
	int* dx = nullptr;
	int* dy = nullptr;
	int blockSize = 4;

	// ���� ȸ�� ������ ������ ����
	SetBlockOffsets(type, rotation, dx, dy);

	// ���� ��� ��ǥ ����
	for (int i = 0; i < blockSize; i++) {
		int curX = startX + dx[i];
		int curY = startY + dy[i];

		if (!CheckBoundary(curY, curX)) {
			cerr << "Block�� ��ȿ������ �����.\n";
			exit(EXIT_FAILURE);
		}

		pair<int, int> curPosPair = make_pair(curY, curX);
		curBlocks.push_back(curPosPair);
		treeMap[curPosPair] = true;
	}

	// ���� ȸ�� ���� ���
	int nextRotation = (rotation + 1) % 4;

	int* next_dx = nullptr;
	int* next_dy = nullptr;
	SetBlockOffsets(type, nextRotation, next_dx, next_dy);

	// ���� ȸ�� ��ǥ ��� �� �˻�
	for (int i = 0; i < blockSize; i++) {
		int nextX = startX + next_dx[i];
		int nextY = startY + next_dy[i];
		nextBlocks.push_back({ nextY, nextX });

		// ��� �˻�
		if (!CheckBoundary(nextY, nextX)) {
			return; // ȸ�� �Ұ�
		}

		// ��ħ �˻� (�ڱ� �ڽ� ����)
		pair<int, int> nextPos = { nextY, nextX };
		if (treeMap.find(nextPos) == treeMap.end() && grid[nextY][nextX] == 1) {
			return; // ȸ�� �Ұ�
		}
	}

	// ---- ������� ���� ȸ�� ���� ---- //

	// ���� ��ġ ����
	for (auto& curBlock : curBlocks) {
		grid[curBlock.first][curBlock.second] = 0;
	}

	// ���ο� ��ġ ä���
	for (auto& nextBlock : nextBlocks) {
		grid[nextBlock.first][nextBlock.second] = 1;
	}

	// ȸ�� ���� ����
	rotation = nextRotation;
}

void RotateCounterClockWiseBlock(BlockType type, int& rotation, int& startX, int& startY) {

	bool moveFlag = true;
	vector<pair<int, int>> nextBlocks;
	vector<pair<int, int>> curBlocks;
	map<pair<int, int>, bool> treeMap;

	// ������ = �迭
	int* dx = nullptr;
	int* dy = nullptr;
	int blockSize = 4;

	// ���� ȸ�� ������ ������ ����
	SetBlockOffsets(type, rotation, dx, dy);

	// ���� ��� ��ǥ ����
	for (int i = 0; i < blockSize; i++) {
		int curX = startX + dx[i];
		int curY = startY + dy[i];

		if (!CheckBoundary(curY, curX)) {
			cerr << "Block�� ��ȿ������ �����.\n";
			exit(EXIT_FAILURE);
		}

		pair<int, int> curPosPair = make_pair(curY, curX);
		curBlocks.push_back(curPosPair);
		treeMap[curPosPair] = true;
	}

	// ���� ȸ�� ���� ���
	int nextRotation = ((rotation - 1) + 4) % 4;

	int* next_dx = nullptr;
	int* next_dy = nullptr;
	SetBlockOffsets(type, nextRotation, next_dx, next_dy);

	// ���� ȸ�� ��ǥ ��� �� �˻�
	for (int i = 0; i < blockSize; i++) {
		int nextX = startX + next_dx[i];
		int nextY = startY + next_dy[i];
		nextBlocks.push_back({ nextY, nextX });

		// ��� �˻�
		if (!CheckBoundary(nextY, nextX)) {
			return; // ȸ�� �Ұ�
		}

		// ��ħ �˻� (�ڱ� �ڽ� ����)
		pair<int, int> nextPos = { nextY, nextX };
		if (treeMap.find(nextPos) == treeMap.end() && grid[nextY][nextX] == 1) {
			return; // ȸ�� �Ұ�
		}
	}

	// ---- ������� ���� ȸ�� ���� ---- //

	// ���� ��ġ ����
	for (auto& curBlock : curBlocks) {
		grid[curBlock.first][curBlock.second] = 0;
	}

	// ���ο� ��ġ ä���
	for (auto& nextBlock : nextBlocks) {
		grid[nextBlock.first][nextBlock.second] = 1;
	}

	// ȸ�� ���� ����
	rotation = nextRotation;
}


BlockType GetRandomBlock() {
	// ���� ���� ���� �ʱ�ȭ
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 6);  // 0~6 ���� ����

	int randomValue = dist(gen);
	return static_cast<BlockType>(randomValue);
}

int GetSpawnX(BlockType type) {
	switch (type) {
	case BlockType::BlockI:
		// I ����� ���� 4ĭ�̹Ƿ� �߾� ���� �� 3���� ����
		return 3;

	case BlockType::BlockO:
		// O ����� ���� 2ĭ
		return 4;

	case BlockType::BlockT:
		// T ��� ���� 3ĭ
		return 3;

	case BlockType::BlockL:
		// L ��� ���� 2ĭ
		return 4;

	case BlockType::BlockJ:
		// J ��� ���� 2ĭ
		return 4;

	case BlockType::BlockS:
		// S ��� ���� 3ĭ
		return 3;

	case BlockType::BlockZ:
		// Z ��� ���� 3ĭ
		return 3;

	default:
		return 3;
	}
}

void HandleKeyInput() {
	char key = 0;

	if (_kbhit()) {        // Ű�� ���ȴ��� Ȯ��
		key = _getch();    // ���� Ű�� ��ȯ
	}

	// �Է��� ������ 0 ��ȯ
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
		// �� ĭ ������
		GravityBlock(curBlockType, curBlockRotation, curBlock_startX, curBlock_startY);
	}

	// ȸ�� Ű ó��
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
		// �ƹ� �ٵ� ���ŵ��� ����
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
		totalScore += 800;  // ��Ʈ����
		break;

	default:
		// 4�� �̻��� �⺻������ 4�� ó��
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

		// �Ʒ��������� ���� �˻�
		for (int i = height - 1; i >= 0; i--) {
			// �� �� �˻�
			bool clearFlag = true;

			for (int j = 0; j < width; j++) {
				if (grid[i][j] == 0) {
					clearFlag = false;
				}
			}

			// �� �� ����
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
	// ȭ�� Ŭ����
	// ���Ŀ� ������۸��� �ؾ� ȭ��������� ���� �� �ִ�.
	system("cls");

	ResetSpawnFlag();

	if (canSpawn) {
		SpawnRandomBlock();
	}

	if (gameOverFlag) {
		cout << "Game Over!\n";
		ShowTotalScore();

		return;		// �� �̻� �������� �ʰ� ����
	}

	ApplyKeyMove();

	ShowGrid();
	ShowTotalScore();


	GravityBlock(curBlockType, curBlockRotation, curBlock_startX, curBlock_startY);


	// ���� Ű ���� �ʱ�ȭ
	KeyManager::GetInstance().Reset();
}

int main() {
	// �ð������׽�Ʈ

	// while ���� �ȿ� while������ �� ������ ������ ���ư���

	int timer = 0;
	grid.clear();
	grid.resize(20, vector<int>(10, 0));


	// ����� ������
	std::ios::sync_with_stdio(false);

	// �ð��� üũ�ϴ� ����
	while (true) {
		timer++;

		HandleKeyInput();

		if (timer >= 6000) {
			// �� �ȿ� ���ӷ����� �������Ѵ�.
			GameLogic();

			timer = 0;
		}
	}
}