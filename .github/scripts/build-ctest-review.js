module.exports = async ({github, context, pull_number, status, log}) => {
    const reviews = await github.rest.pulls.listReviews({
        pull_number: pull_number,
        owner: context.repo.owner,
        repo: context.repo.repo,
    });

    console.log(reviews);

    for (let review of reviews.data) {
        if (review.user.login === 'github-actions' && review.state === 'COMMENTED') {
            await github.rest.pulls.dismissReview({
                pull_number: pull_number,
                owner: context.repo.owner,
                repo: context.repo.repo,
                review_id: review.id
            });
        }
    }

    await github.rest.pulls.createReview({
        pull_number: pull_number,
        owner: context.repo.owner,
        repo: context.repo.repo,
        body: (status === 'success' ? '😍 Tests passed!' : '😨 Tests failed!') + '\n\n```\n' + log.replaceAll('%%%', "\n") + '\n```',
        event: (status === 'success' ? 'APPROVE' : 'REQUEST_CHANGES')
    });
}