#pragma once
class key
{public:
	int primaryKey;
	int RBN;
	int childRBN;
	int parentKeyRBN;

	key(int zip_code)
	{
		primaryKey = zip_code;
	}
};

