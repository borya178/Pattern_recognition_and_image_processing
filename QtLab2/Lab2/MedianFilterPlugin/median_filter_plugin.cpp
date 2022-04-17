#include "median_filter_plugin.h"

QString MedianFilterPlugin::name()
{
    return "Median Filter";
}

void selection_sort(std::vector<int> &arr, int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        int min_index = i;
        for (int j = i + 1; j < size; j++)
        {
            if (arr[j] < arr[min_index])
            {
                min_index = j;
            }
        }
        if (min_index != i)
        {
            std::swap(arr[i], arr[min_index]);
        }
    }
}

void MedianFilterPlugin::edit(const cv::Mat &input, cv::Mat &output)
{
    output = input.clone();

    for(int row = 1; row < input.rows - 1; row++)
    {
        for(int col = 1; col < input.cols - 1; col++)
        {
            std::vector<int> R;
            std::vector<int> G;
            std::vector<int> B;
            for(int r = row - 1; r <= row + 1; r++)
            {
                for(int c = col - 1; c <= col + 1; c++)
                {
                    B.push_back(input.at<cv::Vec3b>(r, c)[0]);
                    G.push_back(input.at<cv::Vec3b>(r, c)[1]);
                    R.push_back(input.at<cv::Vec3b>(r, c)[2]);
                }
            }
            selection_sort(B, 9);
            selection_sort(G, 9);
            selection_sort(R, 9);
            output.at<cv::Vec3b>(row, col)[0] = B[4];
            output.at<cv::Vec3b>(row, col)[1] = G[4];
            output.at<cv::Vec3b>(row, col)[2] = R[4];
        }
    }
}
