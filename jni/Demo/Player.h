//Player info

class Player
{	
	 float mF;
	 int mScore;
public:
	Player() : 
		
		mF(0.01f), 		
		mScore(0)
	
	{;}


	void addToScore(int score) 
	{
		mScore += score;
	}

	void setScore(int score) 
	{
		mScore = score;
	}


	int getScore() 
	{
		return mScore;
	}
	
};
