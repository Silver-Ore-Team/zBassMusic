module.exports = async ({github, context, pull_number, status, log}) => {
    const reviews = await github.rest.pulls.listReviews({
        pull_number: pull_number,
        owner: context.repo.owner,
        repo: context.repo.repo,
    });

    reviews.find(r => r.user.login ==='github-actions' && r.state === 'COMMENTED').forEach(r => {
        github.rest.pulls.dismissReview({
            pull_number: pull_number,
            owner: context.repo.owner,
            repo: context.repo.repo,
            review_id: r.id
        });
    });

    github.rest.pulls.createReview({
        pull_number: pull_number,
        owner: context.repo.owner,
        repo: context.repo.repo,
        body: (status === 'success' ? '😍 Tests passed!' : '😨 Tests failed!') + '\n\n```\n' + log.replaceAll('%%%', "\n") + '\n```',
        event: (status === 'success' ? 'APPROVE' : 'REQUEST_CHANGES')
    });
}