#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

class ProgressBar
{
public:
    void setTotal(int total)
    {
        this->m_total = total;
    }

    bool update(int progress)
    {
        if (progress > m_total)
            return false;

        int real_progress = round(((progress * 1.0)/(m_total*1.0) * 100));
		if (real_progress > 100)
			real_progress = 100;

		output(real_progress);
		return real_progress < 100;
    }

	void end()
	{
		output(100);
	}

private:
	void output(int progress)
	{
		char bar[101] = { 0 };
		for (int i = 0; i < progress; i++)
		{
			bar[i] = '=';
		}

		printf("[\033[32m%-100s\033[0m] %d%%\r", bar, progress);
		fflush(stdout);
		if (progress == 100)
		{
			printf("\n");
		}
	}

private:
    int m_total = 0;
};

#endif